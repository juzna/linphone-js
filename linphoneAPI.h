/**********************************************************\

  Auto-generated linphoneAPI.h

\**********************************************************/

#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>

#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include "private.h" /*coreapi/private.h, needed for LINPHONE_VERSION */
#include <limits.h>
#include <ctype.h>

#include <linphonecore.h>

#include "ortp/ortp.h"

#include "JSAPIAuto.h"
#include "BrowserHostWrapper.h"

#include <pthread.h>

#ifndef H_linphoneAPI
#define H_linphoneAPI


/* Lock class */
class Lock {
public:
 Lock(pthread_mutex_t* plock, char* from) { if(from) printf("locking mutex: "); pthread_mutex_lock(_lock = plock); if(_from = from) printf("locked from %s\n", from); }
 ~Lock() { if(_from) printf("unlocking mutex: %s\n", _from); pthread_mutex_unlock(_lock); }

private:
 pthread_mutex_t* _lock;
 char *_from;
};

#define mmethod(name) make_method(this, &linphoneAPI::name)
#define rmethod(name) registerMethod(#name, make_method(this, &linphoneAPI::call_##name))
#define rmethod2(name, func) registerMethod(#name, make_method(this, &linphoneAPI::call_##func))
#define rproperty(name) registerProperty(#name, make_property(this, &linphoneAPI::get_##name, &linphoneAPI::set_##name))
#define rpropertyg(name) registerProperty(#name, make_property(this, &linphoneAPI::get_##name))
#define mcb(name) cb_##name

class linphoneAPI : public FB::JSAPIAuto
{
public:
    linphoneAPI(FB::BrowserHostWrapper *host);
    virtual ~linphoneAPI();

    // Exported methods
    bool call_start(void);
    bool call_quit(void);
    void call_addAuthInfo(std::string username, std::string realm, std::string password);
    void call_addProxy(std::string proxy, std::string identity);
    bool call_accept(void);
    bool call_terminate(void);
    bool call_call(std::string uri);
    bool call_refer(std::string uri);
    int  call_getHookState(void);
    
    // Property manipulation methods
    bool get_autoAnswer(void);
    void set_autoAnswer(bool);
    bool get_captureVideo(void);
    void set_captureVideo(bool);
    bool get_showVideo(void);
    void set_showVideo(bool);
    bool get_registered(void);
    
    // Linhpone core callbacks
    void lcb_call_received(LinphoneCore *lc, const char *from); // New call received
    void lcb_bye_received(LinphoneCore *lc, const char *from);  // Call is terminated by the other side
    void lcb_notify_received(LinphoneCore *lc, LinphoneFriend *fid, const char *url, const char *status, const char *img);
    void lcb_auth_info_requested(LinphoneCore *lc, const char *realm, const char *username);
    void lcb_display_status(LinphoneCore *lc, const char *message);
    void lcb_display_something(LinphoneCore *lc, const char *message);
    void lcb_display_warning(LinphoneCore *lc, const char *message);
    void lcb_display_url(LinphoneCore * lc, const char *something, const char *url);
    void lcb_text_received(LinphoneCore *lc, LinphoneChatRoom *cr, const char *from, const char *msg);
    void lcb_general_state(LinphoneCore * lc, LinphoneGeneralState *gstate);
    void lcb_dtmf_received(LinphoneCore *lc, int dtmf);
    
    // Other methods
    void log(std::string);

    void lock() { pthread_mutex_lock(&mutex); }
    void unlock() { pthread_mutex_unlock(&mutex); }
    void iterate() { if(lin) linphone_core_iterate(lin); }
    void iterateWithMutex() { lock(); iterate(); unlock(); }
    
    
private:
    FB::AutoPtr<FB::BrowserHostWrapper> m_host;
    bool m_autoAnswer;
    bool m_captureVideo;
    bool m_showVideo;

    LinphoneCore *lin;	// Linphone core object
    LinphoneCoreVTable lin_vtable; // Linphone callback methods table
    pthread_mutex_t mutex; // Mutex for serializing core calls
    ortp_thread_t iterate_thread; // Iterate thread
    std::string callee; // Who do we call
    std::string caller; // Who is calling
    
public: bool iterate_thread_running; // Indicate, when should iterate thread stop
};

static void stub () {}
static void* iterate_thread_main(void *p); // Main function for iterate thread

#endif // H_linphoneAPI

