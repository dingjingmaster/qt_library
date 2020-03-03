#ifndef BIL_H
#define BIL_H

#include <QtCore/qglobal.h>

// 定义BIL_SHARE，使用者可以不用再处理符号的导入和导出细节
#ifdef BIL_LIB
# define BIL_SHARE Q_DECL_EXPORT
#else
# define BIL_SHARE Q_DECL_IMPORT
#endif

#endif // BIL_H
