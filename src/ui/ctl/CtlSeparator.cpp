/*
 * CtlSeparator.cpp
 *
 *  Created on: 9 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlSeparator::CtlSeparator(CtlRegistry *src, LSPSeparator *widget, ssize_t orientation): CtlWidget(src, widget)
        {
            nOrientation = orientation;
        }
        
        CtlSeparator::~CtlSeparator()
        {
        }

        void CtlSeparator::init()
        {
            CtlWidget::init();

            if (pWidget == NULL)
                return;

            LSPSeparator *sep = static_cast<LSPSeparator *>(pWidget);

            // Initialize color controllers
            sColor.init_hsl(pRegistry, sep, sep->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sBgColor.init_basic(pRegistry, sep, sep->bg_color(), A_BG_COLOR);
        }

        void CtlSeparator::set(widget_attribute_t att, const char *value)
        {
            LSPSeparator *sep = (pWidget != NULL) ? static_cast<LSPSeparator *>(pWidget) : NULL;

            switch (att)
            {
                case A_SIZE:
                    if (sep != NULL)
                        PARSE_INT(value, sep->set_size(__));
                    break;
                case A_WIDTH:
                    if (sep != NULL)
                        PARSE_INT(value, sep->set_line_width(__));
                    break;
                case A_BORDER:
                    if (sep != NULL)
                        PARSE_INT(value, sep->set_border(__));
                    break;
                case A_PADDING:
                    if (sep != NULL)
                        PARSE_INT(value, sep->set_padding(__));
                    break;
                case A_HORIZONTAL:
                    if ((sep != NULL) && (nOrientation < 0))
                        PARSE_BOOL(value, sep->set_horizontal(__));
                    break;
                case A_VERTICAL:
                    if ((sep != NULL) && (nOrientation < 0))
                        PARSE_BOOL(value, sep->set_vertical(__));
                    break;
                default:
                {
                    bool set = sColor.set(att, value);
                    set |= sBgColor.set(att, value);

                    if (!set)
                        CtlWidget::set(att, value);
                    break;
                }
            }
        }

    } /* namespace ctl */
} /* namespace lsp */
