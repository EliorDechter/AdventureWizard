
#include "WzLayout.h"

//#define wz_log(x) (void)x;

unsigned int wz_layout_failed;

void wz_log(WzLogMessage* arr, unsigned int* count, const char* fmt, ...)
{
	WzLogMessage message;
	message.str[0] = 0;
	va_list args;
	va_start(args, fmt);
	vsprintf_s(message.str, WZ_LOG_MESSAGE_MAX_SIZE, fmt, args);
	va_end(args);

	arr[*count] = message;
	*count = *count + 1;

	//printf("%s", message.str);
}

void wz_assert(unsigned int b)
{
	if (!b)
	{
		assert(0);
		//wz_layout_failed = 1;
	}
}

void wz_do_layout(unsigned int index,
	WzWidgetData* widgets, WzLayoutRect* rects,
	unsigned int count, unsigned int* failed)
{
	wz_assert(count);

	wz_layout_failed = 0;

	unsigned int widgets_stack_count = 0;

	unsigned int size_per_flex_factor;
	WzWidgetData* widget;
	WzWidgetData* child;

	unsigned int constraint_max_w, constraint_max_h;

	unsigned int w;
	unsigned int h;
	unsigned int available_size_main_axis, available_size_cross_axis;
	int i;
	unsigned int children_flex_factor;
	unsigned int children_size, max_child_h;
	unsigned int children_h, max_child_w;
	unsigned int parent_index;

	const unsigned int max_depth = 32;
	unsigned int* widgets_visits = calloc(sizeof(*widgets_visits), count);
	unsigned int* widgets_stack = calloc(sizeof(*widgets_stack), max_depth);

	WzLogMessage* log_messages = malloc(sizeof(*log_messages) * count * 20);
	unsigned int log_messages_count = 0;

	widgets_stack[widgets_stack_count++] = index;

	unsigned int* constraint_min_main_axis, * constraint_max_main_axis,
		* constraint_min_cross_axis = 0, * constraint_max_cross_axis = 0,
		* actual_size_main_axis, * actual_size_cross_axis;

	unsigned int* child_constraint_min_main_axis, * child_constraint_max_main_axis,
		* child_constraint_min_cross_axis, * child_constraint_max_cross_axis,
		* child_actual_size_main_axis, * child_cross_axis_actual_size;

	unsigned int screen_size_main_axis, screen_size_cross_axis;

	unsigned int parent_cross_axis_size;

	unsigned int padding_cross_axis;
	//WzLayoutRect* child_rect, * widget_rect;
	unsigned int root_w, root_h;

	root_w = widgets[index].constraint_max_w;
	root_h = widgets[index].constraint_max_h;

	// Constraints pass
	while (widgets_stack_count)
	{
		parent_index = widgets_stack[widgets_stack_count - 1];
		widget = &widgets[parent_index];
		//widget_rect = &rects[parent_index];

		//wz_assert(widget->constraint_max_w >= widget->constraint_min_w);
		//wz_assert(widget->constraint_max_h >= widget->constraint_min_h);

		if (!widget->children_count)
		{
			// Size leaf widgets, and pop immediately
			// For now all leaf widgets must have a finite constraint
			// Later on we'll let them decide their own size based on their content
			if (widget->constraint_max_w == WZ_UINT_MAX)
			{
				wz_log(log_messages, &log_messages_count, "(%s) ERROR: Widget width has unbounded constraints\n",
					widget->source);
			}

			if (widget->constraint_max_h == WZ_UINT_MAX)
			{
				wz_log(log_messages, &log_messages_count, "(%s) ERROR: Widget height has unbounded constraints\n",
					widget->source);
			}

			widget->actual_x = 0;
			widget->actual_y = 0;
			widget->actual_w = widget->constraint_max_w;
			widget->actual_h = widget->constraint_max_h;

			if (!widget->actual_w)
			{
				wz_log(log_messages, &log_messages_count, "(%s) ERROR: Widget  width has no constraints\n",
					widget->source);
			}
			if (!widget->actual_h)
			{
				wz_log(log_messages, &log_messages_count,
					"(%s) ERROR: Widget  height has no constraints\n",
					widget->source);
			}

			wz_assert(widget->actual_w <= widget->constraint_max_w);
			wz_assert(widget->actual_h <= widget->constraint_max_h);

			wz_log(log_messages, &log_messages_count,
				"(%s) LOG: Leaf widget with constraints (%u %u %u, %u) determined its size (%u, %u)\n",
				widget->source,
				widget->constraint_min_w, widget->constraint_min_h,
				widget->constraint_max_w, widget->constraint_max_h,
				widget->actual_w, widget->actual_h);

			widgets_stack_count--;
		}
		else
		{
			// Handle widgets with children
			if (widget->layout == WZ_LAYOUT_HORIZONTAL || widget->layout == WZ_LAYOUT_VERTICAL)
			{
				if (widget->layout == WZ_LAYOUT_HORIZONTAL)
				{
					constraint_min_main_axis = &widget->constraint_min_w;
					constraint_max_main_axis = &widget->constraint_max_w;
					constraint_min_cross_axis = &widget->constraint_min_h;
					constraint_max_cross_axis = &widget->constraint_max_h;
					actual_size_main_axis = &widget->actual_w;
					actual_size_cross_axis = &widget->actual_h;
					screen_size_main_axis = root_w;
					screen_size_cross_axis = root_h;
					padding_cross_axis = widget->pad_top + widget->pad_bottom;
				}
				else if (widget->layout == WZ_LAYOUT_VERTICAL)
				{
					constraint_min_main_axis = &widget->constraint_min_h;
					constraint_max_main_axis = &widget->constraint_max_h;
					constraint_min_cross_axis = &widget->constraint_min_w;
					constraint_max_cross_axis = &widget->constraint_max_w;
					actual_size_main_axis = &widget->actual_h;
					actual_size_cross_axis = &widget->actual_w;
					screen_size_main_axis = root_h;
					screen_size_cross_axis = root_w;
					padding_cross_axis = widget->pad_left + widget->pad_right;
				}
				else
				{
					child_constraint_min_main_axis = child_constraint_max_main_axis =
						child_constraint_max_cross_axis = child_constraint_min_cross_axis = 0;
					constraint_max_main_axis = 0;
					actual_size_main_axis = actual_size_cross_axis = 0;

					wz_assert(0);
				}

				// You got 3 visits for layout widget.
				// 1. Non Flex children get fixed constraints
				// 2. Above children determine their desired size, and now we allocate available space to flex children
				// 3. It's the turn of the flex children to determine their size, and then we can finally assess the 
				// Layout widget's size using it's children's 
				if (widgets_visits[parent_index] == WZ_LAYOUT_STAGE_NON_FLEX_CHILDREN)
				{
					// First give cross axis constraints to all children
					const char* widget_type = 0;
					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						widget_type = "Row";
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						widget_type = "Column";
					}

					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: %s with constraints (main %u, cross %u) begins allocating cross axis constraints to children\n",
						widget->source, widget_type,
						*constraint_max_main_axis, *constraint_max_cross_axis);

					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i]];

						{
							for (int i = 0; i < widget->children_count; ++i)
							{
								child = &widgets[widget->children[i]];
								//child_rect = &rects[widget->children[i]];

								if (child->free_from_parent)
								{
									continue;
								}

								if (widget->layout == WZ_LAYOUT_HORIZONTAL)
								{
									child_constraint_min_cross_axis = &child->constraint_min_h;
									child_constraint_max_cross_axis = &child->constraint_max_h;
								}
								else if (widget->layout == WZ_LAYOUT_VERTICAL)
								{
									child_constraint_min_cross_axis = &child->constraint_min_w;
									child_constraint_max_cross_axis = &child->constraint_max_w;
								}
								else
								{
									wz_assert(0);
									child_constraint_min_cross_axis = 0;
									child_constraint_max_cross_axis = 0;
								}

								if (*constraint_max_cross_axis > padding_cross_axis)
								{
									unsigned int constraints = *constraint_max_cross_axis - padding_cross_axis;
									if (constraints < *child_constraint_max_cross_axis)
									{
										*child_constraint_max_cross_axis = constraints;

										if (widget->cross_axis_alignment == WZ_CROSS_AXIS_ALIGNMENT_STRETCH)
										{
											*child_constraint_min_cross_axis = constraints;
										}
									}

									wz_log(log_messages, &log_messages_count,
										"(%s) LOG: Widget recieved cross %u\n",
										child->source, *child_constraint_max_cross_axis);
								}
								else
								{
									*child_constraint_max_cross_axis = 0;
									wz_log(log_messages, &log_messages_count,
										"(%s) ERROR: Flex widget had no space available to it \n",
										child->source);
								}
							}
						}
					}

					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: %s with constraints (main %u, cross %u) ends allocating cross axis constraints to children\n",
						widget->source, widget_type,
						*constraint_max_main_axis, *constraint_max_cross_axis);

					// Give constraints to non flex children
					// A child with flex factor 0 recieves unbounded constraints in the main axis
					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: %s  with constraints (main %u, cross %u) begins constrains non-flex children\n",
						widget->source, widget_type,
						*constraint_max_main_axis, *constraint_max_cross_axis);

					// We leave a non-flex constraints unchanged
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i]];

						if (child->flex_factor == 0)
						{
							if (widget->layout == WZ_LAYOUT_HORIZONTAL)
							{
								child_constraint_min_main_axis = &child->constraint_min_w;
								child_constraint_max_main_axis = &child->constraint_max_w;
								child_constraint_min_cross_axis = &child->constraint_min_h;
								child_constraint_max_cross_axis = &child->constraint_max_h;
							}
							else if (widget->layout == WZ_LAYOUT_VERTICAL)
							{
								child_constraint_min_main_axis = &child->constraint_min_h;
								child_constraint_max_main_axis = &child->constraint_max_h;
								child_constraint_min_cross_axis = &child->constraint_min_w;
								child_constraint_max_cross_axis = &child->constraint_max_w;
							}
							else
							{
								// Just to appease the compiler
								child_constraint_min_main_axis = child_constraint_max_main_axis =
									child_constraint_min_cross_axis = child_constraint_max_cross_axis = 0;
								wz_assert(0);
							}

							wz_log(log_messages, &log_messages_count,
								"(%s) LOG: Non-flex widget with constraints (%u %u %u %u)\n",
								child->source,
								child->constraint_min_w, child->constraint_min_h,
								child->constraint_max_w, child->constraint_max_h);

							widgets_stack[widgets_stack_count] = widget->children[i];
							widgets_stack_count++;
						}
					}

					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: %s ends constrains non-flex children\n",
						widget->source, widget_type);

					widgets_visits[parent_index] = 1;
				}
				else if (widgets_visits[parent_index] == WZ_LAYOUT_STAGE_FLEX_CHILDREN)
				{
					// Give constraints to flex children, allocating from the availble space
					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						available_size_main_axis = widget->constraint_max_w;
						available_size_cross_axis = widget->constraint_max_h;
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						available_size_main_axis = widget->constraint_max_h;
						available_size_cross_axis = widget->constraint_max_w;
					}

					children_flex_factor = 0;

					for (i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i]];
						//child_rect = &rects[widget->children[i]];

						if (child->free_from_parent)
						{
							continue;
						}

						if (widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							child_actual_size_main_axis = &child->actual_w;
						}
						else if (widget->layout == WZ_LAYOUT_VERTICAL)
						{
							child_actual_size_main_axis = &child->actual_h;
						}
						else
						{
							child_actual_size_main_axis = 0;
						}

						if (!child->flex_factor)
						{
							//wz_assert(*child_actual_size_main_axis);
							if (available_size_main_axis >= *child_actual_size_main_axis)
							{
								available_size_main_axis -= *child_actual_size_main_axis;
							}
							else
							{
								available_size_main_axis = 0;
							}
						}
						else
						{
							children_flex_factor += child->flex_factor;
						}
					}

					// Widget allocating space to flex children cannot be unbounded in the main axis
					if (children_flex_factor && constraint_max_main_axis == UINT_MAX)
					{
						wz_log(log_messages, &log_messages_count,
							"(%s) ERROR: Widget allocating space to flex \
							children cannot be unbounded in the main axis\n",
							widget->source);
					}

					// Substract padding and child gap 
					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						if (available_size_main_axis >= widget->pad_left + widget->pad_right)
						{
							available_size_main_axis -= widget->pad_left + widget->pad_right;
						}
						else
						{
							available_size_main_axis = 0;
						}
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						if (available_size_main_axis >= widget->pad_top + widget->pad_bottom)
						{
							available_size_main_axis -= widget->pad_top + widget->pad_bottom;
						}
						else
						{
							available_size_main_axis = 0;
						}
					}

					if (available_size_main_axis > widget->child_gap * (widget->children_count - 1))
					{
						available_size_main_axis -= widget->child_gap * (widget->children_count - 1);
					}
					else
					{
						available_size_main_axis = 0;
					}

					if (children_flex_factor)
					{
						size_per_flex_factor = available_size_main_axis / children_flex_factor;
					}

					const char* widget_type = 0;
					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						widget_type = "Row";
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						widget_type = "Column";
					}

					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: %s begins constrains flex children\n",
						widget->source, widget_type);

					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i]];

						if (widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							child_constraint_min_main_axis = &child->constraint_min_w;
							child_constraint_max_main_axis = &child->constraint_max_w;
							child_constraint_min_cross_axis = &child->constraint_min_h;
							child_constraint_max_cross_axis = &child->constraint_max_h;
						}
						else if (widget->layout == WZ_LAYOUT_VERTICAL)
						{
							child_constraint_min_main_axis = &child->constraint_min_h;
							child_constraint_max_main_axis = &child->constraint_max_h;
							child_constraint_min_cross_axis = &child->constraint_min_w;
							child_constraint_max_cross_axis = &child->constraint_max_w;
						}
						else
						{
							child_constraint_min_main_axis = child_constraint_max_main_axis =
								child_constraint_max_cross_axis = child_constraint_min_cross_axis = 0;
							wz_assert(0);
						}

						if (child->flex_factor)
						{
							unsigned int main_axis_size = size_per_flex_factor * child->flex_factor;

							if (widget->flex_fit == WZ_FLEX_FIT_TIGHT)
							{
								*child_constraint_min_main_axis = main_axis_size;
							}

							*child_constraint_max_main_axis = main_axis_size;

							if (child_constraint_max_main_axis == 0)
							{
								wz_log(log_messages, &log_messages_count,
									"(%s) ERROR: Flex widget had no space available to it \n",
									child->source);
							}

							wz_log(log_messages, &log_messages_count,
								"(%s) LOG: Flex widget recieved constraints (%u, %u) \n",
								child->source,
								child->constraint_max_w, child->constraint_max_h);

							widgets_stack[widgets_stack_count] = widget->children[i];
							widgets_stack_count++;
						}
					}

					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: %s  ends constrains flex children\n",
						widget->source, widget_type);

					widgets_visits[parent_index] = 2;
				}
				else if (widgets_visits[parent_index] == WZ_LAYOUT_STAGE_PARENT)
				{
					// We finally determined the size of all the children of a widget with a layout
					// Now we determine it's size

					wz_assert(widget->children_count);

					// Main axis size
					if (widget->main_axis_size_type == MAIN_AXIS_SIZE_TYPE_MIN)
					{

						if (widget->constraint_max_h == widget->constraint_min_h &&
							widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							wz_log(log_messages, &log_messages_count,
								"(%s) ERROR: Row has tight constraints in it's horizontal axis, but it's supposed to shrink-wrap.\n",
								widget->source);
						}

						if (widget->constraint_max_h == widget->constraint_min_h &&
							widget->layout == WZ_LAYOUT_VERTICAL)
						{
							wz_log(log_messages, &log_messages_count,
								"(%s) ERROR: Column has tight constraints in it's vertical axis, but it's supposed to shrink-wrap.\n",
								widget->source);
						}

						children_size = 0;
						for (i = 0; i < widget->children_count; ++i)
						{
							child = &widgets[widget->children[i]];
							//child_rect = &rects[widget->children[i]];

							if (widget->layout == WZ_LAYOUT_HORIZONTAL)
							{
								child_actual_size_main_axis = &child->actual_w;
							}
							else if (widget->layout == WZ_LAYOUT_VERTICAL)
							{
								child_actual_size_main_axis = &child->actual_h;
							}
							else
							{
								child_actual_size_main_axis = 0;
								wz_assert(0);
							}

							children_size += *child_actual_size_main_axis;
						}

						*actual_size_main_axis = children_size;
					}
					else
					{

						// Layout widget is unconstrained in the main axis
						// It must shrink-wrap
						if (widget->layout == WZ_LAYOUT_HORIZONTAL &&
							widget->constraint_max_w == WZ_UINT_MAX)
						{
							wz_log(log_messages, &log_messages_count,
								"(%s) ERROR: Row recieved unbounded constraints in the horizontal axis\n",
								widget->source);
						}

						if (widget->layout == WZ_LAYOUT_VERTICAL &&
							widget->constraint_max_h == WZ_UINT_MAX)
						{
							wz_log(log_messages, &log_messages_count,
								"(%s) ERROR: Column recieved unbounded constraints in the vertical axis\n",
								widget->source);
						}

						// Determine size of widget that is constrained in the main axis
						*actual_size_main_axis = *constraint_max_main_axis;
					}

					// Cross axis size (use tallest child)
					parent_cross_axis_size = 0;
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i]];
						//child_rect = &rects[widget->children[i]];

						if (widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							if (child->actual_h > parent_cross_axis_size)
							{
								parent_cross_axis_size = child->actual_h;
							}
						}
						else if (widget->layout == WZ_LAYOUT_VERTICAL)
						{
							if (child->actual_w > parent_cross_axis_size)
							{
								parent_cross_axis_size = child->actual_w;
							}
						}
					}


					{
						if (!parent_cross_axis_size)
						{
							wz_log(log_messages, &log_messages_count,
								"(%s) LOG: widget has not room for children in the cross axis\n", widget->source);
						}
						*actual_size_cross_axis = parent_cross_axis_size;
					}
					
					widget->actual_h += widget->pad_top + widget->pad_bottom;
					widget->actual_w += widget->pad_left + widget->pad_right;


					// Clamp
					if (widget->actual_h < widget->constraint_min_h)
					{
						widget->actual_h = widget->constraint_min_h;
					}

					if (widget->actual_h > widget->constraint_max_h)
					{
						widget->actual_h = widget->constraint_max_h;
					}

					if (widget->actual_w < widget->constraint_min_w)
					{
						widget->actual_w = widget->constraint_min_w;
					}

					if (widget->actual_w > widget->constraint_max_w)
					{
						widget->actual_w = widget->constraint_max_w;
					}

					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						wz_log(log_messages, &log_messages_count,
							"(%s) LOG: Row widget  with constraints (%u, %u) determined its size (%u, %u)\n",
							widget->source,
							widget->constraint_max_w, widget->constraint_max_h, widget->actual_w, widget->actual_h);
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						wz_log(log_messages, &log_messages_count,
							"(%s) LOG: Column widget  with constraints (%u, %u) determined its size (%u, %u)\n",
							widget->source,
							widget->constraint_max_w, widget->constraint_max_h, widget->actual_w, widget->actual_h);
					}

					wz_assert(*actual_size_main_axis <= *constraint_max_main_axis);
					wz_assert(*actual_size_cross_axis <= *constraint_max_cross_axis);

					// Give positions to children
					unsigned int offset = 0;
					widget->actual_x = 0;
					widget->actual_y = 0;
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i]];
						//child_rect = &rects[widget->children[i]];

						if (child->free_from_parent)
						{
							continue;
						}

						if (widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							child_actual_size_main_axis = &child->actual_w;
							child->actual_x= offset;
							child->actual_y = 0;
						}
						else if (widget->layout == WZ_LAYOUT_VERTICAL)
						{
							child_actual_size_main_axis = &child->actual_h;
							child->actual_y = offset;
							child->actual_x= 0;
						}
						else
						{
							child_actual_size_main_axis = 0;
							wz_assert(0);
						}

						// Position padding
						child->actual_x+= widget->pad_left;
						child->actual_y += widget->pad_top;

						offset += *child_actual_size_main_axis;
						offset += widget->child_gap;

						wz_log(log_messages, &log_messages_count,
							"(%s) LOG: Child widget (%u) will have the raltive position %u %u\n",
							child->source, widget->children[i],
							child->actual_x, child->actual_y);
					}

					widgets_stack_count--;
				}
				else
				{
					wz_assert(0);
					}
			}
			else if (widget->layout == WZ_LAYOUT_NONE)
			{
#if 1
				if (widgets_visits[parent_index] == 0)
				{
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i]];
						//child_rect = &rects[widget->children[i]];

						constraint_max_w = widget->constraint_max_w - (widget->pad_left + widget->pad_right);
						constraint_max_h = widget->constraint_max_h - (widget->pad_top + widget->pad_bottom);

						if (constraint_max_w < child->constraint_max_w)
						{
							child->constraint_max_w = constraint_max_h;
						}
						if (constraint_max_h < child->constraint_max_h)
						{
							child->constraint_max_h = constraint_max_h;
						}

						child->actual_x = widget->pad_left;
						child->actual_y = widget->pad_right;

						widgets_stack[widgets_stack_count] = widget->children[i];
						widgets_stack_count++;
						widgets_visits[parent_index] = 1;
						wz_log(log_messages, &log_messages_count, "(%s) LOG: Non-layout widget  passes to child constraints (%u, %u) and position (%u %u)\n",
							child->source,
							child->constraint_max_w, child->constraint_max_h, child->actual_x, child->actual_y);
					}
				}
				else if (widgets_visits[parent_index] == 1)
				{
					widget->actual_x = 0;
					widget->actual_y = 0;
					widget->actual_w = widget->constraint_max_w;
					widget->actual_h = widget->constraint_max_h;

					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: Non-layout widget  with constraints (%u, %u) determined its size (%u, %u)\n",
						widget->source,
						widget->constraint_max_w, widget->constraint_max_h, widget->actual_w, widget->actual_h);

					//wz_assert(widget->actual_w <= widget->constraint_max_w);
					//wz_assert(widget->actual_h <= widget->constraint_max_h);
					//wz_assert(widget->actual_w <= root_w);
					//wz_assert(widget->actual_h <= root_h);

					widgets_stack_count--;
				}
#endif
			}
			else
			{
				wz_assert(0);
			}

		}
	}

	// Final stage. Calculate the widgets non-relative final position and cull 
	for (int i = 1; i < count; ++i)
	{
		widget = &widgets[i];
		//widget_rect = &rects[i];
		unsigned int offset = 0;
		unsigned int children_size = 0;

		widget->actual_x += widget->x;
		widget->actual_y += widget->y;

		for (int j = 0; j < widget->children_count; ++j)
		{
			child = &widgets[widget->children[j]];
			//child_rect = &rects[widget->children[j]];

			if (widget->layout == WZ_LAYOUT_HORIZONTAL)
			{
				children_size += child->actual_w;
			}
			else if (widget->layout == WZ_LAYOUT_VERTICAL)
			{
				children_size += child->actual_h;
			}
		}

		children_size += widget->child_gap * (widget->children_count - 1);

		for (int j = 0; j < widget->children_count; ++j)
		{
			child = &widgets[widget->children[j]];
			//child_rect = &rects[widget->children[j]];

			if (!widget->actual_w || !widget->actual_h)
			{
				child->actual_w = 0;
				child->actual_h = 0;
				continue;
			}

			unsigned int parent_size_h = widget->actual_h - widget->pad_top - widget->pad_bottom;
			unsigned int parent_size_w = widget->actual_w - widget->pad_left - widget->pad_right;

			if (widget->cross_axis_alignment == CROSS_AXIS_ALIGNMENT_CENTER)
			{
				if (widget->layout == WZ_LAYOUT_HORIZONTAL && parent_size_h > child->actual_h)
				{
					child->actual_y += (parent_size_h - child->actual_h) / 2;
				}
				else if (widget->layout == WZ_LAYOUT_VERTICAL && parent_size_w > child->actual_w)
				{
					child->actual_x+= (parent_size_w - child->actual_w) / 2;
				}
			}
			else if (widget->cross_axis_alignment == CROSS_AXIS_ALIGNMENT_START)
			{
				// Do nothing
			}

			child->actual_x = widget->actual_x + child->actual_x;
			child->actual_y = widget->actual_y + child->actual_y;

			// Check the widgets size doesnt exceeds its parents
			if (child->actual_x+ child->actual_w >= widget->actual_x + widget->actual_w)
			{
				wz_log(log_messages, &log_messages_count, "(%s) ERROR: Widget exceeds it's parents horizontally\n",
					child->source);
			}
			if (child->actual_y + child->actual_h >= widget->actual_y + widget->actual_h)
			{
				wz_log(log_messages, &log_messages_count, "(%s) ERROR: Widget exceeds it's parents vertically\n",
					child->source);
			}
		}
	}

	wz_log(log_messages, &log_messages_count, "---------------------------\n");
	wz_log(log_messages, &log_messages_count, "Final Layout:\n");

	for (unsigned int i = 0; i < count; ++i)
	{
		widget = &widgets[i];
		//widget_rect = &rects[i];

		wz_log(log_messages, &log_messages_count, "(%u %s %u) : (%d %d %u %u)\n",
			i, widget->source, widget->actual_x, widget->actual_y, widget->actual_w, widget->actual_h);
	}
	wz_log(log_messages, &log_messages_count, "---------------------------\n");

	free(widgets_visits);
	free(widgets_stack);
	free(log_messages);

	*failed = wz_layout_failed;
}
