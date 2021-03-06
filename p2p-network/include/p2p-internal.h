#define ASIO_STANDALONE
#define ASIO_MSVC _MSC_VER
#define ASIO_NO_DEPRECATED
#define ASIO_HAS_CONSTEXPR
#define ASIO_HAS_VARIADIC_TEMPLATES
#define _P2P_NAMESPACE_BEGIN namespace chainthings { namespace p2p {
#define _P2P_NAMESPACE_END } }

#if defined(_MSC_VER) && defined(_DEBUG)
#define DEBUG_MSG
#define DEBUG_CLIENT_SEND_PING
#endif //_DEBUG

#define	MSG_TYPE_PING	0x0001
#define	MSG_TYPE_PONG	0x0002

#include <asio.hpp>
