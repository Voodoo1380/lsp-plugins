/*
 * serialize.h
 *
 *  Created on: 21 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_SERIALIZE_H_
#define UI_SERIALIZE_H_

#include <ui/tk/tk.h>

namespace lsp
{
    /** Load theme from XML
     *
     * @param theme theme to store data
     * @param path path to the file
     * @return true if theme was loaded
     */
    status_t    load_theme(tk::LSPTheme *theme, const char *path);
}

#endif /* UI_SERIALIZE_H_ */
