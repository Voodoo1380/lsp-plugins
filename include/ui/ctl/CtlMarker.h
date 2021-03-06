/*
 * CtlMarker.h
 *
 *  Created on: 27 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLMARKER_H_
#define UI_CTL_CTLMARKER_H_

namespace lsp
{
    namespace ctl
    {
        class CtlMarker: public CtlWidget
        {
            protected:
                CtlPort        *pPort;
                CtlColor        sColor;
                float           fTransparency;

            protected:
                static status_t     slot_change(LSPWidget *sender, void *ptr, void *data);
                void                submit_values();

            public:
                explicit CtlMarker(CtlRegistry *src, LSPMarker *mark);
                virtual ~CtlMarker();

            public:
                /** Begin initialization of controller
                 *
                 */
                virtual void init();

                /** Set attribute
                 *
                 * @param att widget attribute
                 * @param value widget value
                 */
                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLMARKER_H_ */
