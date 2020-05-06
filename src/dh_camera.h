#if !defined(DH_CAMERA)
#define DH_CAMERA

#include <string>
#include "UserDefine.h"
#include "Ptz.h"
#include <ros/ros.h>
#include <alhua_ptz_camera/PtzCtrl.h>
#include <alhua_ptz_camera/PtzStatus.h>
#include <alhua_ptz_camera/WifiStatus.h>


namespace dh_camera
{
using namespace std;

class DhCamera{

    public:
        DhCamera();
        ~DhCamera();
        void run();
    private:
        long _loginHandle;
        long _playHandle;
        CPtz _ptzCtrl;
        DH_Channel_Info _dwUser;
        void initialize();
        bool login(string ip_address, short port, string user_name, string passwd);
        bool startWatching();
        bool stopWatching();
        void logout();
        void cleanup();

        void convertLoginError2String(int nErrorCode , string &strErrorCode);

        ros::NodeHandle _nh;
        ros::ServiceServer _ptz_ctrl_service;
        ros::ServiceServer _ptz_status_service;
        ros::ServiceServer _wifi_status_service;
        bool ptzCtrlCB(alhua_ptz_camera::PtzCtrl::Request  &req, alhua_ptz_camera::PtzCtrl::Response &res);
        bool ptzStatCB(alhua_ptz_camera::PtzStatus::Request  &req, alhua_ptz_camera::PtzStatus::Response &res);
        bool wifiStatCB(alhua_ptz_camera::WifiStatus::Request  &req, alhua_ptz_camera::WifiStatus::Response &res);
};



}




#endif // DH_CAMERA
