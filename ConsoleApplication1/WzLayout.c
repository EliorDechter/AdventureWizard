
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
		//wz_layout_failed = 1;
	}
}

void wz_do_layout(unsigned int index,
	WzWidgetDescriptor* widgets, WzLayoutRect* rects,
	unsigned int count, unsigned int *failed)
{
	wz_layout_failed = 0;

	unsigned int widgets_stack_count = 0;

	unsigned int size_per_flex_factor;
	WzWidgetDescriptor* parent;
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
		* constraint_min_cross_axis = 0, * parent_constraint_max_cross_axis = 0,
		* actual_size_main_axis, * actual_size_cross_axis;

	unsigned int* child_constraint_min_main_axis, * child_constraint_max_main_axis,
		* child_constraint_min_cross_axis, * child_constraint_max_cross_axis,
		* child_actual_size_main_axis, * child_cross_axis_actual_size;

	unsigned int screen_size_main_axis, screen_size_cross_axis;

	unsigned int parent_cross_axis_size;

	unsigned int padding_cross_axis;
	WzLayoutRect* child_rect, * parent_rect;
	unsigned int root_w, root_h;

	root_w = widgets[index].constraint_max_w;
	root_h = widgets[index].constraint_max_h;

	rects[index].x = 0;
	rects[index].y = 0;

	// Constraints pass
	while (widgets_stack_count)
	{
		parent_index = widgets_stack[widgets_stack_count - 1];
		parent = &widgets[parent_index];
		parent_rect = &rects[parent_index];

		wz_assert(parent->constraint_max_w > 0);
		wz_assert(parent->constraint_max_h > 0);
		wz_assert(parent_rect->x >= 0);
		wz_assert(parent_rect->y >= 0);

		if (!parent->children_count)
		{
			// Size leaf widgets, and pop immediately
			// For now all leaf widgets must have a finite constraint
			// Later on we'll let them decide their own size based on their content

#if 0
			WZ_ASSERT(!((parent->layout == WZ_LAYOUT_HORIZONTAL || parent->layout == WZ_LAYOUT_VERTICAL))
			&& !parent->children_count);
#endif

			wz_assert(parent->constraint_max_w != WZ_UINT_MAX);
			parent_rect->w = parent->constraint_max_w;

			wz_assert(parent->constraint_max_h != WZ_UINT_MAX);
			parent_rect->h = parent->constraint_max_h;

			wz_assert(parent_rect->w);
			wz_assert(parent_rect->h);
			wz_assert(parent_rect->w <= root_w);
			wz_assert(parent_rect->h <= root_h);
			wz_assert(parent_rect->w <= parent->constraint_max_w);
			wz_assert(parent_rect->h <= parent->constraint_max_h);
			wz_assert(parent_rect->w <= root_w);
			wz_assert(parent_rect->h <= root_h);

			wz_log(log_messages, &log_messages_count, "Leaf widget (%u %s %d) with constraints (%u, %u) determined its size (%u, %u)\n",
				parent_index, parent->file, parent->line,
				parent->constraint_max_w, parent->constraint_max_h, parent_rect->w, parent_rect->h);

			widgets_stack_count--;
		}
		else
		{
			// Handle widgets with children
			if (parent->layout == WZ_LAYOUT_HORIZONTAL || parent->layout == WZ_LAYOUT_VERTICAL)
			{
				if (parent->layout == WZ_LAYOUT_HORIZONTAL)
				{
					constraint_min_main_axis = &parent->constraint_min_w;
					constraint_max_main_axis = &parent->constraint_max_w;
					constraint_min_cross_axis = &parent->constraint_min_h;
					parent_constraint_max_cross_axis = &parent->constraint_max_h;
					actual_size_main_axis = &parent_rect->w;
					actual_size_cross_axis = &parent_rect->h;
					screen_size_main_axis = root_w;
					screen_size_cross_axis = root_h;
					padding_cross_axis = parent->pad_top + parent->pad_bottom;
				}
				else if (parent->layout == WZ_LAYOUT_VERTICAL)
				{
					constraint_min_main_axis = &parent->constraint_min_h;
					constraint_max_main_axis = &parent->constraint_max_h;
					constraint_min_cross_axis = &parent->constraint_min_w;
					parent_constraint_max_cross_axis = &parent->constraint_max_w;
					actual_size_main_axis = &parent_rect->h;
					actual_size_cross_axis = &parent_rect->w;
					screen_size_main_axis = root_h;
					screen_size_cross_axis = root_w;
					padding_cross_axis = parent->pad_left + parent->pad_right;
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
					if (parent->layout == WZ_LAYOUT_HORIZONTAL)
					{
						widget_type = "Row";
					}
					else if (parent->layout == WZ_LAYOUT_VERTICAL)
					{
						widget_type = "Column";
					}

					wz_log(log_messages, &log_messages_count, "%s (%s, %d) with constraints (main %u, cross %u) begins constrains non-flex children\n",
						widget_type, parent->file, parent->line,
						*constraint_max_main_axis, *parent_constraint_max_cross_axis);

					// Give constraints to non flex children
					// A child with flex factor 0 recieves unbounded constraints in the main axis
					for (int i = 0; i < parent->children_count; ++i)
					{
						child = &widgets[parent->children[i]];

						if (child->flex_factor == 0)
						{
							if (parent->layout == WZ_LAYOUT_HORIZONTAL)
							{
								child_constraint_min_main_axis = &child->constraint_min_w;
								child_constraint_max_main_axis = &child->constraint_max_w;
								child_constraint_min_cross_axis = &child->constraint_min_h;
								child_constraint_max_cross_axis = &child->constraint_max_h;
							}
							else if (parent->layout == WZ_LAYOUT_VERTICAL)
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
								// Leave by default all constraints as they are
								// Child will only inherit the cross axis constraint
								wz_assert(*child_constraint_max_cross_axis);
								if (*parent_constraint_max_cross_axis < *child_constraint_max_cross_axis)
								{
									wz_assert(*parent_constraint_max_cross_axis > padding_cross_axis);
									*child_constraint_max_cross_axis = *parent_constraint_max_cross_axis - padding_cross_axis;
								}

								wz_assert(*child_constraint_max_main_axis);
								wz_assert(*child_constraint_max_cross_axis);

								wz_log(log_messages, &log_messages_count, "Non-flex widget (%s, %d) with constraints (%u, %u)\n", child->file, child->line,
									child->constraint_max_w, child->constraint_max_h);
							}

							widgets_stack[widgets_stack_count] = parent->children[i];
							widgets_stack_count++;
						}
					}
					wz_log(log_messages, &log_messages_count, "%s (%s, %d) ends constrains non-flex children\n", widget_type, parent->file, parent->line);

					widgets_visits[parent_index] = 1;
				}
				else if (widgets_visits[parent_index] == 1)
				{
					// Give constraints to flex children, allocating from the availble space
					if (parent->layout == WZ_LAYOUT_HORIZONTAL)
					{
						available_size_main_axis = parent->constraint_max_w;
						available_size_cross_axis = parent->constraint_max_h;
					}
					else if (parent->layout == WZ_LAYOUT_VERTICAL)
					{
						available_size_main_axis = parent->constraint_max_h;
						available_size_cross_axis = parent->constraint_max_w;
					}

					children_flex_factor = 0;

					for (i = 0; i < parent->children_count; ++i)
					{
						child = &widgets[parent->children[i]];
						child_rect = &rects[parent->children[i]];

						if (child->free_from_parent_horizontally)
						{
							continue;
						}

						if (parent->layout == WZ_LAYOUT_HORIZONTAL)
						{
							child_actual_size_main_axis = &child_rect->w;
						}
						else if (parent->layout == WZ_LAYOUT_VERTICAL)
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
					if (parent->layout == WZ_LAYOUT_HORIZONTAL)
					{
						if (available_size_main_axis >= parent->pad_left + parent->pad_right)
						{
							available_size_main_axis -= parent->pad_left + parent->pad_right;
						}
						else
						{
							available_size_main_axis = 0;
						}
					}
					else if (parent->layout == WZ_LAYOUT_VERTICAL)
					{
						if (available_size_main_axis >= parent->pad_top + parent->pad_bottom)
						{
							available_size_main_axis -= parent->pad_top + parent->pad_bottom;
						}
						else
						{
							available_size_main_axis = 0;
						}
					}

					if (available_size_main_axis > parent->gap * (parent->children_count - 1))
					{
						available_size_main_axis -= parent->gap * (parent->children_count - 1);
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
					if (parent->layout == WZ_LAYOUT_HORIZONTAL)
					{
						widget_type = "Row";
					}
					else if (parent->layout == WZ_LAYOUT_VERTICAL)
					{
						widget_type = "Column";
					}

					wz_log(log_messages, &log_messages_count, "%s (%s, %d) begins constrains flex children\n", widget_type, parent->file, parent->line);

					for (int i = 0; i < parent->children_count; ++i)
					{
						child = &widgets[parent->children[i]];

						if (parent->layout == WZ_LAYOUT_HORIZONTAL)
						{
							child_constraint_min_main_axis = &child->constraint_min_w;
							child_constraint_max_main_axis = &child->constraint_max_w;
							child_constraint_min_cross_axis = &child->constraint_min_h;
							child_constraint_max_cross_axis = &child->constraint_max_h;
						}
						else if (parent->layout == WZ_LAYOUT_VERTICAL)
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

							if (parent->flex_fit == WZ_FLEX_FIT_TIGHT)
							{
								*child_constraint_min_main_axis = main_axis_size;
							}
							if (main_axis_size < *child_constraint_max_main_axis)
							{
								*child_constraint_max_main_axis = main_axis_size;
							}

							if (*constraint_min_cross_axis < *child_constraint_min_cross_axis)
							{
								*child_constraint_min_cross_axis = *constraint_min_cross_axis;
							}

							if (*parent_constraint_max_cross_axis < *child_constraint_max_cross_axis)
							{
								*child_constraint_max_cross_axis = *parent_constraint_max_cross_axis - padding_cross_axis;
							}

							wz_log(log_messages, &log_messages_count, "Flex widget (%s, %d) recieved constraints (%u, %u) \n", child->file, child->line,
								child->constraint_max_w, child->constraint_max_h);

							widgets_stack[widgets_stack_count] = parent->children[i];
							widgets_stack_count++;
						}
					}

					wz_log(log_messages, &log_messages_count, "%s (%s, %d) ends constrains flex children\n", widget_type, parent->file, parent->line);

					widgets_visits[parent_index] = 2;
				}
				else if (widgets_visits[parent_index] == 2)
				{
					// We finally determined the size of all the children of a widget with a layout
					// Now we determine it's size

					wz_assert(parent->children_count);

					// Layout widget is unconstrained in the main axis
					// It must shrink-wrap
					wz_assert(!(parent->layout == WZ_LAYOUT_HORIZONTAL &&
						parent->constraint_max_w == WZ_UINT_MAX &&
						parent->main_axis_size_type == MAIN_AXIS_SIZE_TYPE_MAX));
					wz_assert(!(parent->layout == WZ_LAYOUT_VERTICAL &&
						parent->constraint_max_h == WZ_UINT_MAX &&
						parent->main_axis_size_type == MAIN_AXIS_SIZE_TYPE_MAX));

					// Main axis size
					if (*constraint_max_main_axis == WZ_UINT_MAX)
					{
						children_size = 0;
						for (i = 0; i < parent->children_count; ++i)
						{
							child = &widgets[parent->children[i]];
							child_rect = &rects[parent->children[i]];

							if (parent->layout == WZ_LAYOUT_HORIZONTAL)
							{
								child_actual_size_main_axis = &child_rect->w;
							}
							else if (parent->layout == WZ_LAYOUT_VERTICAL)
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
					for (int i = 0; i < parent->children_count; ++i)
					{
						child = &widgets[parent->children[i]];
						child_rect = &rects[parent->children[i]];

						wz_assert(child_rect->w <= root_w);
						wz_assert(child_rect->h <= root_h);

						if (parent->layout == WZ_LAYOUT_HORIZONTAL)
						{
							if (child_rect->h > parent_cross_axis_size)
							{
								parent_cross_axis_size = child_rect->h;
							}
						}
						else if (parent->layout == WZ_LAYOUT_VERTICAL)
						{
							if (child_rect->w > parent_cross_axis_size)
							{
								parent_cross_axis_size = child_rect->w;
							}
						}
					}
					
					if (parent->layout == WZ_LAYOUT_HORIZONTAL)
					{
						parent_cross_axis_size += parent->pad_top + parent->pad_bottom;
					}
					else if (parent->layout == WZ_LAYOUT_VERTICAL)
					{
						parent_cross_axis_size += parent->pad_left+ parent->pad_right;
					}


					wz_assert(parent_cross_axis_size);
					wz_assert(parent_cross_axis_size <= parent_constraint_max_cross_axis);


					*actual_size_cross_axis = parent_cross_axis_size;

					if (parent->layout == WZ_LAYOUT_HORIZONTAL)
					{
						wz_log(log_messages, &log_messages_count, "Row widget (%s, %d) with constraints (%u, %u) determined its size (%u, %u)\n", parent->file, parent->line,
							parent->constraint_max_w, parent->constraint_max_h, parent_rect->w, parent_rect->h);
					}
					else if (parent->layout == WZ_LAYOUT_VERTICAL)
					{
						wz_log(log_messages, &log_messages_count, "Column widget (%s, %d) with constraints (%u, %u) determined its size (%u, %u)\n", parent->file, parent->line,
							parent->constraint_max_w, parent->constraint_max_h, parent_rect->w, parent_rect->h);
					}

					wz_assert(*actual_size_main_axis <= *constraint_max_main_axis);
					wz_assert(*actual_size_cross_axis <= *parent_constraint_max_cross_axis);
					wz_assert(*actual_size_main_axis <= screen_size_main_axis);
					wz_assert(*actual_size_cross_axis <= screen_size_cross_axis);

					// Give positions to children
					unsigned int offset = 0;
					for (int i = 0; i < parent->children_count; ++i)
					{
						child = &widgets[parent->children[i]];
						child_rect = &rects[parent->children[i]];

						if (child->free_from_parent_horizontally)
						{
							continue;
						}

						if (parent->layout == WZ_LAYOUT_HORIZONTAL)
						{
							child_actual_size_main_axis = &child_rect->w;
							child_rect->x = offset;
							child_rect->y = 0;
						}
						else if (parent->layout == WZ_LAYOUT_VERTICAL)
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
						child_rect->x += parent->pad_left;
						child_rect->y += parent->pad_top;

						offset += *child_actual_size_main_axis;
						offset += parent->gap;

						wz_log(log_messages, &log_messages_count, "Child widget (%u %s, %d) will have the raltive position %u %u\n",
							parent->children[i], child->file, child->line,
							child_rect->x, child_rect->y);
						
						//wz_assert(child_rect->x + child_rect->w <= parent_rect->w - parent->pad_right);
						//wz_assert(child_rect->y + child_rect->h <= parent_rect->h - parent->pad_bottom);
						wz_assert(child_rect->x <= root_w);
						wz_assert(child_rect->y <= root_h);
					}
					
					widgets_stack_count--;
				}
			}
			else if (parent->layout == WZ_LAYOUT_NONE)
			{
				if (widgets_visits[parent_index] == 0)
				{
					for (int i = 0; i < parent->children_count; ++i)
					{
						child = &widgets[parent->children[i]];
						child_rect = &rects[parent->children[i]];

						constraint_max_w = parent->constraint_max_w - (parent->pad_left + parent->pad_right);
						constraint_max_h = parent->constraint_max_h - (parent->pad_top + parent->pad_bottom);

						if (constraint_max_w < child->constraint_max_w)
						{
							child->constraint_max_w = constraint_max_h;
						}
						if (constraint_max_h < child->constraint_max_h)
						{
							child->constraint_max_h = constraint_max_h;
						}

						child_rect->x = parent->pad_left;
						child_rect->y = parent->pad_right;

						widgets_stack[widgets_stack_count] = parent->children[i];
						widgets_stack_count++;
						widgets_visits[parent_index] = 1;
						wz_log(log_messages, &log_messages_count, "Non-layout widget (%s, %d) passes to child constraints (%u, %u) and position (%u %u)\n",
							child->file, child->line,
							child->constraint_max_w, child->constraint_max_h, child_rect->x, child_rect->y);
					}
				}
				else if (widgets_visits[parent_index] == 1)
				{

					parent_rect->w = parent->constraint_max_w;
					parent_rect->h = parent->constraint_max_h;

					wz_log(log_messages, &log_messages_count,
						"Non-layout widget (%s, %d) with constraints (%u, %u) determined its size (%u, %u)\n",
						parent->file, parent->line,
						parent->constraint_max_w, parent->constraint_max_h, parent_rect->w, parent_rect->h);

					wz_assert(parent_rect->w <= parent->constraint_max_w);
					wz_assert(parent_rect->h <= parent->constraint_max_h);
					wz_assert(parent_rect->w <= root_w);
					wz_assert(parent_rect->h <= root_h);

					widgets_stack_count--;
				}
			}
			else
			{
				wz_assert(0);
			}
		}
	}

	// Final stage. Calculate the widgets non-relative final position 
	for (int i = 1; i < count; ++i)
	{
		parent = &widgets[i];
		parent_rect = &rects[i];
		unsigned int offset = 0;
		unsigned int children_size = 0;

		for (int j = 0; j < parent->children_count; ++j)
		{
			child = &widgets[parent->children[j]];
			child_rect = &rects[parent->children[j]];

			if (parent->layout == WZ_LAYOUT_HORIZONTAL)
			{
				children_size += child_rect->w;
			}
			else if (parent->layout == WZ_LAYOUT_VERTICAL)
			{
				children_size += child_rect->h;
			}
		}

		children_size += parent->gap * (parent->children_count - 1);

		for (int j = 0; j < parent->children_count; ++j)
		{
			child = &widgets[parent->children[j]];
			child_rect = &rects[parent->children[j]];

			unsigned int parent_size_h = parent_rect->h - parent->pad_top - parent->pad_bottom;
			unsigned int parent_size_w = parent_rect->w - parent->pad_left - parent->pad_right;

			if (parent->alignment & WzAlignHCenter)
			{
				if (parent->layout == WZ_LAYOUT_HORIZONTAL)
				{
					child_rect->x += (parent_size_w - children_size) / 2;
				}
				else
				{
					child_rect->x += (parent_size_w - child_rect->w) / 2;
				}
			}
			
			if (parent->alignment & WzAlignVCenter)
			{
				if (parent->layout == WZ_LAYOUT_VERTICAL)
				{
					child_rect->y += (parent_size_h - children_size) / 2;
				}
				else
				{
					child_rect->y += (parent_size_h - child_rect->h) / 2;
				}
			}

			child_rect->x = parent_rect->x + child_rect->x;
			child_rect->y = parent_rect->y + child_rect->y;

			// Check the widgets size doesnt exceeds its parents
			wz_assert(child_rect->x >= 0);
			wz_assert(child_rect->y >= 0);
			wz_assert(child_rect->y >= 0);
			wz_assert(child_rect->w);
			wz_assert(child_rect->h);
			wz_assert(child_rect->w);
			wz_assert(child_rect->h);

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
		parent = &widgets[i];
		parent_rect = &rects[i];

		wz_log(log_messages, &log_messages_count, "(%s %u) : (%u %u %u %u)\n",
			parent->file, parent->line, parent_rect->x, parent_rect->y, parent_rect->w, parent_rect->h);
	}
	wz_log(log_messages, &log_messages_count, "---------------------------\n");


	for (int i = 0; i < log_messages_count; ++i)
	{
		//printf("%s", log_messages[i].str);
	}

	free(widgets_visits);
	free(widgets_stack);
	free(log_messages);

	*failed = wz_layout_failed;
}
