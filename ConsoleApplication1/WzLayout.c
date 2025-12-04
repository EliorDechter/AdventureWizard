
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

	printf("%s", message.str);
}

WzWidgetDescriptor wz_widget_descriptor_create(
	unsigned int constraint_min_w, unsigned int constraint_min_h,
	unsigned int constraint_max_w, unsigned int constraint_max_h,
	unsigned int layout,
	unsigned int pad_left, unsigned int pad_right, unsigned int pad_top, unsigned int pad_bottom,
	unsigned int* children,
	unsigned int children_count,
	unsigned char free_from_parent_horizontally, unsigned char free_from_parent_vertically,
	unsigned char flex_fit
)
{

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
	WzWidgetDescriptor* widgets, WzLayoutRect* rects,
	unsigned int count, unsigned int* failed)
{
	wz_layout_failed = 0;

	unsigned int widgets_stack_count = 0;

	unsigned int size_per_flex_factor;
	WzWidgetDescriptor* widget;
	WzWidgetDescriptor* child;

	unsigned int constraint_max_w, constraint_max_h;

	unsigned int w;
	unsigned int h;
	unsigned int available_size_main_axis, available_size_cross_axis;
	int i;
	unsigned int children_flex_factor;
	unsigned int children_size, max_child_h;
	unsigned int children_h, max_child_w;
	unsigned int parent_index;

	const unsigned int max_depth = 10;
	unsigned int* widgets_visits = calloc(sizeof(*widgets_visits), count);
	unsigned int* widgets_stack = calloc(sizeof(*widgets_stack), max_depth);

	WzLogMessage* log_messages = malloc(sizeof(*log_messages) * count * 10);
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
	WzLayoutRect* child_rect, * widget_rect;
	unsigned int root_w, root_h;

	root_w = widgets[index].constraint_max_w;
	root_h = widgets[index].constraint_max_h;

	rects[index].x = 0;
	rects[index].y = 0;

	// Constraints pass
	while (widgets_stack_count)
	{
		parent_index = widgets_stack[widgets_stack_count - 1];
		widget = &widgets[parent_index];
		widget_rect = &rects[parent_index];

		//wz_assert(widget->constraint_max_w >= widget->constraint_min_w);
		//wz_assert(widget->constraint_max_h >= widget->constraint_min_h);

		if (!widget->children_count)
		{
			// Size leaf widgets, and pop immediately
			// For now all leaf widgets must have a finite constraint
			// Later on we'll let them decide their own size based on their content

			wz_assert(widget->constraint_max_w != WZ_UINT_MAX);
			widget_rect->w = widget->constraint_max_w;

			wz_assert(widget->constraint_max_h != WZ_UINT_MAX);
			widget_rect->h = widget->constraint_max_h;

			wz_assert(widget_rect->w);
			wz_assert(widget_rect->h);
			//wz_assert(widget_rect->w <= root_w);
			//wz_assert(widget_rect->h <= root_h);
			wz_assert(widget_rect->w <= widget->constraint_max_w);
			wz_assert(widget_rect->h <= widget->constraint_max_h);

			wz_log(log_messages, &log_messages_count, "Leaf widget (%u %s %d) with constraints (%u %u %u, %u) determined its size (%u, %u)\n",
				parent_index, widget->file, widget->line,
				widget->constraint_min_w, widget->constraint_min_h,
				widget->constraint_max_w, widget->constraint_max_h,
				widget_rect->w, widget_rect->h);

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
					actual_size_main_axis = &widget_rect->w;
					actual_size_cross_axis = &widget_rect->h;
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
					actual_size_main_axis = &widget_rect->h;
					actual_size_cross_axis = &widget_rect->w;
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
				if (widgets_visits[parent_index] == 0)
				{
					const char* widget_type = 0;
					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						widget_type = "Row";
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						widget_type = "Column";
					}

					wz_log(log_messages, &log_messages_count, "%s (%s, %d) with constraints (main %u, cross %u) begins constrains non-flex children\n",
						widget_type, widget->file, widget->line,
						//*constraint_min_main_axis, *constraint_min_cross_axis,
						*constraint_max_main_axis, *constraint_max_cross_axis);

					// Give constraints to non flex children
					// A child with flex factor 0 recieves unbounded constraints in the main axis
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

							if (!child->free_from_parent_horizontally)
							{
								// Child recieves unbounded constraints on the main axis, and its parents costraint on the cross axis

								if (*constraint_max_cross_axis < *child_constraint_max_cross_axis)
								{
									if (*constraint_max_cross_axis > padding_cross_axis)
									{
										*child_constraint_max_cross_axis = *constraint_max_cross_axis - padding_cross_axis;
									}
									else
									{
										*child_constraint_max_cross_axis = 0;
									}
								}

								//*child_constraint_max_main_axis = UINT_MAX;

								wz_log(log_messages, &log_messages_count, "Non-flex widget (%s, %d) with constraints (%u %u %u %u)\n", child->file, child->line,
									child->constraint_min_w, child->constraint_min_h,
									child->constraint_max_w, child->constraint_max_h);
							}

							widgets_stack[widgets_stack_count] = widget->children[i];
							widgets_stack_count++;
						}
					}
					wz_log(log_messages, &log_messages_count, "%s (%s, %d) ends constrains non-flex children\n", widget_type, widget->file, widget->line);

					widgets_visits[parent_index] = 1;
				}
				else if (widgets_visits[parent_index] == 1)
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
						child_rect = &rects[widget->children[i]];

						if (child->free_from_parent_horizontally)
						{
							continue;
						}

						if (widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							child_actual_size_main_axis = &child_rect->w;
						}
						else if (widget->layout == WZ_LAYOUT_VERTICAL)
						{
							child_actual_size_main_axis = &child_rect->h;
						}
						else
						{
							child_actual_size_main_axis = 0;
						}

						if (!child->flex_factor)
						{
							wz_assert(*child_actual_size_main_axis);
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

					if (available_size_main_axis > widget->gap * (widget->children_count - 1))
					{
						available_size_main_axis -= widget->gap * (widget->children_count - 1);
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

					wz_log(log_messages, &log_messages_count, "%s (%s, %d) begins constrains flex children\n", widget_type, widget->file, widget->line);

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
							if (main_axis_size < *child_constraint_max_main_axis)
							{
								*child_constraint_max_main_axis = main_axis_size;
							}

#if 0
							if (*constraint_min_cross_axis < *child_constraint_min_cross_axis)
							{
								*child_constraint_min_cross_axis = *constraint_min_cross_axis;
							}

							if (*parent_constraint_max_cross_axis < *child_constraint_max_cross_axis)
							{
								*child_constraint_max_cross_axis = *parent_constraint_max_cross_axis - padding_cross_axis;
							}
#endif

							if (widget->cross_axis_alignment == CROSS_AXIS_ALIGNMENT_STRETCH)
							{
								*child_constraint_max_cross_axis = *constraint_max_cross_axis - padding_cross_axis;
							}

							wz_log(log_messages, &log_messages_count, "Flex widget (%s, %d) recieved constraints (%u, %u) \n", child->file, child->line,
								child->constraint_max_w, child->constraint_max_h);

							widgets_stack[widgets_stack_count] = widget->children[i];
							widgets_stack_count++;
						}
					}

					wz_log(log_messages, &log_messages_count, "%s (%s, %d) ends constrains flex children\n", widget_type, widget->file, widget->line);

					widgets_visits[parent_index] = 2;
				}
				else if (widgets_visits[parent_index] == 2)
				{
					// We finally determined the size of all the children of a widget with a layout
					// Now we determine it's size

					wz_assert(widget->children_count);

					// Layout widget is unconstrained in the main axis
					// It must shrink-wrap
					wz_assert(!(widget->layout == WZ_LAYOUT_HORIZONTAL &&
						widget->constraint_max_w == WZ_UINT_MAX &&
						widget->main_axis_size_type == MAIN_AXIS_SIZE_TYPE_MAX));
					wz_assert(!(widget->layout == WZ_LAYOUT_VERTICAL &&
						widget->constraint_max_h == WZ_UINT_MAX &&
						widget->main_axis_size_type == MAIN_AXIS_SIZE_TYPE_MAX));

					// Main axis size
					if (*constraint_max_main_axis == WZ_UINT_MAX)
					{
						children_size = 0;
						for (i = 0; i < widget->children_count; ++i)
						{
							child = &widgets[widget->children[i]];
							child_rect = &rects[widget->children[i]];

							if (widget->layout == WZ_LAYOUT_HORIZONTAL)
							{
								child_actual_size_main_axis = &child_rect->w;
							}
							else if (widget->layout == WZ_LAYOUT_VERTICAL)
							{
								child_actual_size_main_axis = &child_rect->h;
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
						// Determine size of widget that is constrained in the main axis
						*actual_size_main_axis = *constraint_max_main_axis;
					}

					// Cross axis size (use tallest child)
					parent_cross_axis_size = 0;
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i]];
						child_rect = &rects[widget->children[i]];

						//wz_assert(child_rect->w <= root_w);
						//wz_assert(child_rect->h <= root_h);

						if (widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							if (child_rect->h > parent_cross_axis_size)
							{
								parent_cross_axis_size = child_rect->h;
							}
						}
						else if (widget->layout == WZ_LAYOUT_VERTICAL)
						{
							if (child_rect->w > parent_cross_axis_size)
							{
								parent_cross_axis_size = child_rect->w;
							}
						}
					}

					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						parent_cross_axis_size += widget->pad_top + widget->pad_bottom;
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						parent_cross_axis_size += widget->pad_left + widget->pad_right;
					}

					*actual_size_cross_axis = parent_cross_axis_size;

					if (widget_rect->h < widget->constraint_min_h)
					{
						widget_rect->h = widget->constraint_min_h;
					}

					if (widget_rect->h > widget->constraint_max_h)
					{
						widget_rect->h = widget->constraint_max_h;
					}

					if (widget_rect->w < widget->constraint_min_w)
					{
						widget_rect->w = widget->constraint_min_w;
					}

					if (widget_rect->w > widget->constraint_max_w)
					{
						widget_rect->w = widget->constraint_max_w;
					}

					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						wz_log(log_messages, &log_messages_count, "Row widget (%s, %d) with constraints (%u, %u) determined its size (%u, %u)\n", widget->file, widget->line,
							widget->constraint_max_w, widget->constraint_max_h, widget_rect->w, widget_rect->h);
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						wz_log(log_messages, &log_messages_count, "Column widget (%s, %d) with constraints (%u, %u) determined its size (%u, %u)\n", widget->file, widget->line,
							widget->constraint_max_w, widget->constraint_max_h, widget_rect->w, widget_rect->h);
					}

					wz_assert(*actual_size_main_axis <= *constraint_max_main_axis);
					wz_assert(*actual_size_cross_axis <= *constraint_max_cross_axis);
					//wz_assert(*actual_size_main_axis <= screen_size_main_axis);
					//wz_assert(*actual_size_cross_axis <= screen_size_cross_axis);

					// Give positions to children
					unsigned int offset = 0;
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i]];
						child_rect = &rects[widget->children[i]];

						if (child->free_from_parent_horizontally)
						{
							continue;
						}

						if (widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							child_actual_size_main_axis = &child_rect->w;
							child_rect->x = offset;
							child_rect->y = 0;
						}
						else if (widget->layout == WZ_LAYOUT_VERTICAL)
						{
							child_actual_size_main_axis = &child_rect->h;
							child_rect->y = offset;
							child_rect->x = 0;
						}
						else
						{
							child_actual_size_main_axis = 0;
							wz_assert(0);
						}

						// Position padding
						child_rect->x += widget->pad_left;
						child_rect->y += widget->pad_top;

						offset += *child_actual_size_main_axis;
						offset += widget->gap;

						wz_log(log_messages, &log_messages_count, "Child widget (%u %s, %d) will have the raltive position %u %u\n",
							widget->children[i], child->file, child->line,
							child_rect->x, child_rect->y);

						//wz_assert(child_rect->x + child_rect->w <= parent_rect->w - parent->pad_right);
						//wz_assert(child_rect->y + child_rect->h <= parent_rect->h - parent->pad_bottom);
						//wz_assert(child_rect->x <= root_w);
						//wz_assert(child_rect->y <= root_h);
					}

					widgets_stack_count--;
				}
			}
			else if (widget->layout == WZ_LAYOUT_NONE)
			{
				if (widgets_visits[parent_index] == 0)
				{
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i]];
						child_rect = &rects[widget->children[i]];

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

						child_rect->x = widget->pad_left;
						child_rect->y = widget->pad_right;

						widgets_stack[widgets_stack_count] = widget->children[i];
						widgets_stack_count++;
						widgets_visits[parent_index] = 1;
						wz_log(log_messages, &log_messages_count, "Non-layout widget (%s, %d) passes to child constraints (%u, %u) and position (%u %u)\n",
							child->file, child->line,
							child->constraint_max_w, child->constraint_max_h, child_rect->x, child_rect->y);
					}
				}
				else if (widgets_visits[parent_index] == 1)
				{

					widget_rect->w = widget->constraint_max_w;
					widget_rect->h = widget->constraint_max_h;

					wz_log(log_messages, &log_messages_count,
						"Non-layout widget (%s, %d) with constraints (%u, %u) determined its size (%u, %u)\n",
						widget->file, widget->line,
						widget->constraint_max_w, widget->constraint_max_h, widget_rect->w, widget_rect->h);

					wz_assert(widget_rect->w <= widget->constraint_max_w);
					wz_assert(widget_rect->h <= widget->constraint_max_h);
					wz_assert(widget_rect->w <= root_w);
					wz_assert(widget_rect->h <= root_h);

					widgets_stack_count--;
				}
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
		widget_rect = &rects[i];
		unsigned int offset = 0;
		unsigned int children_size = 0;

		widget_rect->x += widget->x;
		widget_rect->y += widget->y;

		for (int j = 0; j < widget->children_count; ++j)
		{
			child = &widgets[widget->children[j]];
			child_rect = &rects[widget->children[j]];

			if (widget->layout == WZ_LAYOUT_HORIZONTAL)
			{
				children_size += child_rect->w;
			}
			else if (widget->layout == WZ_LAYOUT_VERTICAL)
			{
				children_size += child_rect->h;
			}
		}

		children_size += widget->gap * (widget->children_count - 1);

		for (int j = 0; j < widget->children_count; ++j)
		{
			child = &widgets[widget->children[j]];
			child_rect = &rects[widget->children[j]];

			if (!widget_rect->w || !widget_rect->h)
			{
				child_rect->w = 0;
				child_rect->h = 0;
				continue;
			}

			unsigned int parent_size_h = widget_rect->h - widget->pad_top - widget->pad_bottom;
			unsigned int parent_size_w = widget_rect->w - widget->pad_left - widget->pad_right;

			if (widget->cross_axis_alignment == CROSS_AXIS_ALIGNMENT_CENTER)
			{
				if (widget->layout == WZ_LAYOUT_HORIZONTAL && parent_size_w > children_size)
				{
					child_rect->x += (parent_size_w - child_rect->w) / 2;
				}
				else if (widget->layout == WZ_LAYOUT_VERTICAL && parent_size_h > children_size)
				{
					child_rect->y += (parent_size_h - child_rect->h) / 2;
				}
			}
			else if (widget->cross_axis_alignment == CROSS_AXIS_ALIGNMENT_START)
			{
				// Do nothing
			}

			child_rect->x = widget_rect->x + child_rect->x;
			child_rect->y = widget_rect->y + child_rect->y;

			// Check the widgets size doesnt exceeds its parents
			wz_assert(child_rect->x >= 0);
			wz_assert(child_rect->y >= 0);
			wz_assert(child_rect->y >= 0);
			//wz_assert(child_rect->w);
			//wz_assert(child_rect->h);

			/*wz_assert(child_rect->x <= root_w);
			wz_assert(child_rect->y <= root_h);
			wz_assert(child_rect->x + child_rect->w <= parent_rect->x + parent_rect->w);
			wz_assert(child_rect->y + child_rect->h <= parent_rect->y + parent_rect->h);*/
		}
	}

	wz_log(log_messages, &log_messages_count, "---------------------------\n");
	wz_log(log_messages, &log_messages_count, "Final Layout:\n");
	for (int i = 0; i < count; ++i)
	{
		widget = &widgets[i];
		widget_rect = &rects[i];

		wz_log(log_messages, &log_messages_count, "(%s %u) : (%u %u %u %u)\n",
			widget->file, widget->line, widget_rect->x, widget_rect->y, widget_rect->w, widget_rect->h);
	}
	wz_log(log_messages, &log_messages_count, "---------------------------\n");

	free(widgets_visits);
	free(widgets_stack);
	free(log_messages);

	*failed = wz_layout_failed;
}
