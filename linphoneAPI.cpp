/**********************************************************\

  Auto-generated linphoneAPI.cpp

\**********************************************************/

#include "BrowserObjectAPI.h"
#include "variant_list.h"
#include "DOM/JSAPI_DOMDocument.h"

static void * instance = NULL;

#include "linphoneAPI.h"

// Global callbacks which wraps linphoneAPI object methods
#define GLC if(!linphone_core_get_user_data(lc)) printf("not found linphone api\n"); else ((linphoneAPI*) linphone_core_get_user_data(lc))
// #define GLC ((linphoneAPI*) instance)
static void cb_lcb_call_received(LinphoneCore *lc, const char *from) { GLC->lcb_call_received(lc, from); }
static void cb_lcb_bye_received(LinphoneCore *lc, const char *from) { GLC->lcb_bye_received(lc, from); }
static void cb_lcb_notify_received(LinphoneCore *lc, LinphoneFriend *fid, const char *url, const char *status, const char *img) { GLC->lcb_notify_received(lc, fid, url, status, img); }
static void cb_lcb_auth_info_requested(LinphoneCore *lc, const char *realm, const char *username) { GLC->lcb_auth_info_requested(lc, realm, username); }
static void cb_lcb_display_status(LinphoneCore *lc, const char *message) { GLC->lcb_display_status(lc, message); }

static void cb_lcb_display_something(LinphoneCore *lc, const char *message) { GLC->lcb_display_something(lc, message); }
static void cb_lcb_display_warning(LinphoneCore *lc, const char *message) { GLC->lcb_display_warning(lc, message); }
static void cb_lcb_display_url(LinphoneCore *lc, const char *something, const char *url) { GLC->lcb_display_url(lc, something, url); }
static void cb_lcb_text_received(LinphoneCore *lc, LinphoneChatRoom *cr, const char *from, const char *msg) { GLC->lcb_text_received(lc, cr, from, msg); }
static void cb_lcb_general_state(LinphoneCore *lc, LinphoneGeneralState *gstate) { GLC->lcb_general_state(lc, gstate); }
static void cb_lcb_dtmf_received(LinphoneCore *lc, int dtmf) { GLC->lcb_dtmf_received(lc, dtmf); }



linphoneAPI::linphoneAPI(FB::BrowserHostWrapper *host) : m_host(host)
{
  printf("creating new plugin instance\n");
  
  // Register exported methods
  rmethod(start);
  rmethod(quit);
  rmethod(addAuthInfo);
  rmethod(addProxy);
  rmethod(accept);
  rmethod(terminate);
  rmethod(call);
  rmethod(refer);
  rmethod(getHookState);
  
  // Register properties
  rproperty(autoAnswer);
  rproperty(captureVideo);
  rproperty(showVideo);
  rpropertyg(registered);
  
  // Register events
  registerEvent("onAuthInfoRequested");
  registerEvent("onMessage");
  registerEvent("onStatus");
  registerEvent("onWarning");
  registerEvent("onURL");
  registerEvent("onCall");
  registerEvent("onHangUp");
  registerEvent("onDTMF");
  registerEvent("onStateChange");
  registerEvent("onLog");
  registerEvent("onProxyAdded");
  
  // Initialize mutex
  pthread_mutex_init(&mutex, NULL);  
  
  // Initialize as null pointer
  lin = NULL;
  iterate_thread = NULL;
}

linphoneAPI::~linphoneAPI()
{
  printf("deallocating plugin instance\n");
  
  // Quit first
  call_quit();
}

/**
* Initialie linphone core and start iterate thread
*/
bool linphoneAPI::call_start(void) {
    if(lin) return false; // Already initialized
      
    Lock lck(&mutex, NULL);
    
    // Initialize callback table
    memset(&lin_vtable, 0, sizeof(LinphoneCoreVTable));
    lin_vtable.show 			= (ShowInterfaceCb) stub;
    lin_vtable.inv_recv 		= mcb(lcb_call_received);
    lin_vtable.bye_recv 		= mcb(lcb_bye_received);
    lin_vtable.notify_recv 		= (NotifyReceivedCb) stub;
    lin_vtable.new_unknown_subscriber 	= (NewUnknownSubscriberCb) stub;
    lin_vtable.auth_info_requested 	= mcb(lcb_auth_info_requested);
    lin_vtable.display_status		= mcb(lcb_display_status);
    lin_vtable.display_message		= mcb(lcb_display_something);
    lin_vtable.display_warning		= mcb(lcb_display_warning);
    lin_vtable.display_url		= mcb(lcb_display_url);
    lin_vtable.display_question		= (DisplayQuestionCb) stub;
    lin_vtable.call_log_updated		= (CallLogUpdated) stub;
    lin_vtable.text_received		= mcb(lcb_text_received);
    lin_vtable.general_state		= mcb(lcb_general_state);
    lin_vtable.dtmf_received		= mcb(lcb_dtmf_received);
    lin_vtable.refer_received		= (ReferReceived) stub;
    lin_vtable.buddy_info_updated	= (BuddyInfoUpdated) stub;
    
    char configfile_name[PATH_MAX];
    snprintf(configfile_name, PATH_MAX, "%s/.linphonerc", getenv("HOME"));
    
    // Create linphone core
    instance = this;
    lin = linphone_core_new(&lin_vtable, NULL, NULL, (void *) this);
    if(linphone_core_get_user_data(lin) != this) {
      printf("you have old version of linphone core\n");
      return false;
    }
    
    // Disable/enable logs
    linphone_core_disable_logs();
    //linphone_core_enable_logs(stdout);
    
    linphone_core_set_firewall_policy(lin, LINPHONE_POLICY_USE_STUN);
    linphone_core_set_stun_server(lin, "stun.helemik.cz");
    
    // Initialize iterating thread
    iterate_thread_running = true;
    ortp_thread_create(&iterate_thread,NULL, iterate_thread_main, this);
    
    return true;
}

/**
* Thread, which iterates in linphone core each 20ms
*/
static void *iterate_thread_main(void*p){
    linphoneAPI *t = (linphoneAPI*) p; // Get main object
    printf("iterate thread started\n");
    
    while(t->iterate_thread_running) {
      t->iterateWithMutex();
      usleep(20000);
    }
    printf("iterate thread stopped\n");
}


/**
* Quit linphone core: stop iterate thread
*/
bool linphoneAPI::call_quit(void) {
  if(!lin) return false;
  
  {
    Lock lck(&mutex, "terminate call");
    
    // Terminate call
    linphone_core_terminate_call(lin, NULL);
  }
  
  // Stop iterating
  iterate_thread_running = false;
  ortp_thread_join(iterate_thread,NULL);
  printf("iterate thread joined\n");
  
  // Destroy linphone core
  linphone_core_destroy(lin);
  lin = NULL;
  
  return true;
}

/**
* Add authentication info
*/
void linphoneAPI::call_addAuthInfo(std::string username, std::string realm, std::string passwd) {
  Lock lck(&mutex, "add auth info");
  LinphoneAuthInfo *info;
  
  info = linphone_auth_info_new(username.c_str(), NULL, passwd.c_str(), NULL, realm.c_str());
  linphone_core_add_auth_info(lin, info);
  linphone_auth_info_destroy(info);
}

/**
* Add proxy server
*/
void linphoneAPI::call_addProxy(std::string proxy, std::string identity) {
  {
    Lock lck(&mutex, "add proxy");
    
    LinphoneProxyConfig *cfg;
    cfg = linphone_proxy_config_new();
    
    linphone_proxy_config_set_identity(cfg, identity.c_str());
    linphone_proxy_config_set_server_addr(cfg, proxy.c_str());
    linphone_proxy_config_enable_register(cfg, TRUE);

    // finish the config
    linphone_core_add_proxy_config(lin, cfg);

    // set config as default proxy
    linphone_core_set_default_proxy(lin, cfg);
  }
    
  //FireEvent("onProxyAdded", FB::variant_list_of(proxy)(identity));
}

/**
* Accept incoming call
*/
bool linphoneAPI::call_accept(void) {
  Lock lck(&mutex, "accept");
  return linphone_core_accept_call(lin, NULL) != -1;
}

/**
* Terminate actual call
*/
bool linphoneAPI::call_terminate(void) {
  Lock lck(&mutex, "terminate");
  return linphone_core_terminate_call(lin, NULL) != -1;
}

/**
* Initialize new call
*/
bool linphoneAPI::call_call(std::string uri) {
  Lock lck(&mutex, "call");
  if(-1 == linphone_core_invite(lin, uri.c_str())) {
    return false;
  }
  else {
    callee = uri;
    return false;
  }
}

/**
* Refer call to someone else
*/
bool linphoneAPI::call_refer(std::string uri) {
  return linphone_core_refer(lin, uri.c_str()) != -1;
}

/**
* Get hook state
*/
int  linphoneAPI::call_getHookState(void) {
  Lock lck(&mutex, "get hook state");
  gstate_t call_state = linphone_core_get_state(lin, GSTATE_GROUP_CALL);
  return call_state;
}

// Property manipulation methods
bool linphoneAPI::get_autoAnswer(void) {
  return m_autoAnswer;
}

void linphoneAPI::set_autoAnswer(bool x) {
  m_autoAnswer = x;
}

bool linphoneAPI::get_captureVideo(void) {
  return m_captureVideo;
}

void linphoneAPI::set_captureVideo(bool x) {
 m_captureVideo = x;
 linphone_core_enable_video(lin, m_captureVideo, m_showVideo);
}

bool linphoneAPI::get_showVideo(void) {
 return m_showVideo;  
}

void linphoneAPI::set_showVideo(bool x) {
 m_showVideo = x;
 linphone_core_enable_video(lin, m_captureVideo, m_showVideo); 
}

bool linphoneAPI::get_registered(void) {
  Lock lck(&mutex, "get-registered");
  
  LinphoneProxyConfig *cfg;
  int ret;
  
  linphone_core_get_default_proxy(lin, &cfg); // Get default proxy
  if(!cfg) {
    printf("get registered: no proxy present\n");
    return false;
  }
  ret = linphone_proxy_config_is_registered(cfg);
  
  printf("get registered: got cfg %u; %d\n", cfg, ret);

  if(cfg) return ret > 0;
  else return false;
}


/*
* Callbacks
*/


void linphoneAPI::lcb_call_received(LinphoneCore *lc, const char *from) {
  caller = from;
  if(m_autoAnswer) linphone_core_accept_call(lin, NULL); // Auto accept

  printf("new call from %s\n", from);
    
  // Fire event
  FireEvent("onCall", FB::variant_list_of(caller)(m_autoAnswer));
}

void linphoneAPI::lcb_bye_received(LinphoneCore *lc, const char *from) {
  printf("hanged up from %s\n", from);
  
  // Fire event
  FireEvent("onHangUp", FB::variant_list_of(std::string(from)));
}

void linphoneAPI::lcb_notify_received(LinphoneCore *lc, LinphoneFriend *fid, const char *url, const char *status, const char *img) {}
void linphoneAPI::lcb_auth_info_requested(LinphoneCore *lc, const char *realm, const char *username) {
  printf("auth info requested for %s on %s\n", username, realm);
  FireEvent("onAuthInfoRequested", FB::variant_list_of(realm)(username));
}
void linphoneAPI::lcb_display_status(LinphoneCore *lc, const char *message) {
  printf("status warning: %s\n", message);
  FireEvent("onStatus", FB::variant_list_of(message));
}
void linphoneAPI::lcb_display_something(LinphoneCore *lc, const char *message) {
  printf("something: %s\n", message);
  //FireEvent("onStatus", FB::variant_list_of(message));
}
void linphoneAPI::lcb_display_warning(LinphoneCore *lc, const char *message) {
  printf("Linphone warning: %s\n", message);
  FireEvent("onWarning", FB::variant_list_of(message));
}
void linphoneAPI::lcb_display_url(LinphoneCore * lc, const char *something, const char *url) {
  FireEvent("onURL", FB::variant_list_of(something)(url));
}
void linphoneAPI::lcb_text_received(LinphoneCore *lc, LinphoneChatRoom *cr, const char *from, const char *msg) {
 FireEvent("onMessage", FB::variant_list_of(from)(msg));
}
void linphoneAPI::lcb_general_state(LinphoneCore *lc, LinphoneGeneralState *gstate) {
  int state = gstate->new_state;
  printf("new state: %u\n", state);
  if(gstate->message) printf(" %s\n", gstate->message);
  FireEvent("onStateChange", FB::variant_list_of(state));
}
void linphoneAPI::lcb_dtmf_received(LinphoneCore *lc, int dtmf) {
  printf("DTMF received: %d\n", dtmf);
  FireEvent("onDTMF", FB::variant_list_of(dtmf));
}



// Other methods
void linphoneAPI::log(std::string) {
 // TODO: do log 
}
