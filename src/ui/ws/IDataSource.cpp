/*
 * IDataSource.cpp
 *
 *  Created on: 16 авг. 2019 г.
 *      Author: sadko
 */

#include <ui/ws/ws.h>

namespace lsp
{
    namespace ws
    {
        
        IDataSource::IDataSource(char *const *mimes)
        {
            nReferences     = 0;
            size_t n        = 1;

            for (char *const *m=mimes; *m != NULL; ++m)
                ++n;

            vMimes          = reinterpret_cast<char **>(::malloc(sizeof(char *) * n));
            for (size_t i=0; i<n; ++i)
                vMimes[i]       = NULL;

            for (size_t i=0, j=0; i<n; ++i)
            {
                vMimes[j]   = ::strdup(mimes[i]);
                if (vMimes[j])
                    ++j;
            }
        }
        
        IDataSource::~IDataSource()
        {
            if (vMimes != NULL)
            {
                for (char **p = vMimes; *p != NULL; ++p)
                    ::free(*p);
                ::free(vMimes);
                vMimes = NULL;
            }
        }

        io::IInStream *IDataSource::open(const char *mime)
        {
            return NULL;
        }

        size_t IDataSource::acquire()
        {
            return ++nReferences;
        }

        size_t IDataSource::release()
        {
            ssize_t refs    = --nReferences;
            if (refs <= 0)
                delete this;

            return refs;
        }
    
    } /* namespace ctl */
} /* namespace lsp */
