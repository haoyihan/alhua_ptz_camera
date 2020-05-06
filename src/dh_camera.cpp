#include "dh_camera.h"
#include "dhnetsdk.h"
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <alhua_ptz_camera/PtzCtrl.h>
#include <alhua_ptz_camera/PtzStatus.h>
#include <alhua_ptz_camera/WifiStatus.h>


namespace dh_camera
{

    DhCamera::DhCamera(){
        initialize();

    }

    DhCamera::~DhCamera(){
        cleanup();
    }

    void CALLBACK disConnect(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser){
        printf("Camera Disconnected!\n");
    }

    void DhCamera::initialize(){
        _loginHandle = 0;
        _playHandle = 0;
        _ptz_ctrl_service = _nh.advertiseService("ptz_ctrl",&DhCamera::ptzCtrlCB,this);
        _ptz_status_service = _nh.advertiseService("ptz_status",&DhCamera::ptzStatCB,this);
        _wifi_status_service = _nh.advertiseService("wifi_status",&DhCamera::wifiStatCB,this);
        CLIENT_Init(disConnect, (LDWORD)this);
    }

    bool DhCamera::ptzCtrlCB(alhua_ptz_camera::PtzCtrl::Request  &req, alhua_ptz_camera::PtzCtrl::Response &res){
        switch(req.type){
            case (alhua_ptz_camera::PtzCtrl::Request::CAMERA_RESET):
                _ptzCtrl.PtzExactGo(0,0,0);
                ros::Duration(8.0).sleep();
                printf("0 degree.\n");
                _ptzCtrl.PtzExactGo(3599,900,0);
                ros::Duration(8.0).sleep();
                printf("3600 degree.\n");
                _ptzCtrl.PtzExactGo(0,0,0);
                ros::Duration(8.0).sleep();
                printf("0 degree.\n");
                _ptzCtrl.PtzExactGo(2425,100,0);
                ros::Duration(8.0).sleep();
                printf("180 degree.\n");
                res.success = true;
                res.status_message = "CAMERA RESET";
                break;
            case (alhua_ptz_camera::PtzCtrl::Request::FAST_GO):
                int nx,ny,nZoom;
                nx = req.param_1;
                ny = req.param_2;
                nZoom = req.param_3;
                _ptzCtrl.PtzFastGo(nx,ny,nZoom);
                res.success = true;
                res.status_message = "Fast GO";
                break;
            case (alhua_ptz_camera::PtzCtrl::Request::EXACT_GO):
                _ptzCtrl.PtzExactGo(req.param_1,req.param_2,req.param_3);
                res.success = true;
                res.status_message = "Exact GO";
                break;
            case (alhua_ptz_camera::PtzCtrl::Request::ADD_APERTURE):
                _ptzCtrl.PtzBasicControl(DH_PTZ_APERTURE_ADD_CONTROL,req.param_1,false);
                res.success = true;
                res.status_message = "ADD APERTURE";
                break;
            case (alhua_ptz_camera::PtzCtrl::Request::DEC_APERTURE):
                _ptzCtrl.PtzBasicControl(DH_PTZ_APERTURE_DEC_CONTROL,req.param_1,false);
                res.success = true;
                res.status_message = "DEC APERTURE";
                break;
            case (alhua_ptz_camera::PtzCtrl::Request::ADD_FOCUS):
                _ptzCtrl.PtzBasicControl(DH_PTZ_FOCUS_ADD_CONTROL,req.param_1,false);
                res.success = true;
                res.status_message = "ADD FOCUS";
                break;
            case (alhua_ptz_camera::PtzCtrl::Request::DEC_FOCUS):
                _ptzCtrl.PtzBasicControl(DH_PTZ_FOCUS_DEC_CONTROL,req.param_1,false);
                res.success = true;
                res.status_message = "DEC FOCUS";
                break;
            default:
                return false;

        }
        return true;
    }

    bool DhCamera::ptzStatCB(alhua_ptz_camera::PtzStatus::Request  &req, alhua_ptz_camera::PtzStatus::Response &res){
        DH_PTZ_LOCATION_INFO location_info;
        int ret_len = 0,query_time = 0;
        do{
            query_time ++;
            CLIENT_QueryDevState(_loginHandle,DH_DEVSTATE_PTZ_LOCATION,(char*)&location_info,sizeof(location_info),&ret_len);
        }while(location_info.nPTZPan==0 && location_info.nPTZTilt==0 && location_info.nPTZZoom==0 && query_time<=5 );
        res.nPTZPan = location_info.nPTZPan;
        res.nPTZTilt = location_info.nPTZTilt;
        res.nPTZZoom = location_info.nPTZZoom;
        return true;
    }

    bool DhCamera::wifiStatCB(alhua_ptz_camera::WifiStatus::Request  &req, alhua_ptz_camera::WifiStatus::Response &res){
        DHDEV_VEHICLE_WIFI_STATE wifi_info;
        int ret_len = 0;
        CLIENT_QueryDevState(_loginHandle,DH_DEVSTATE_VICHILE_STATE,(char*)&wifi_info,sizeof(wifi_info),&ret_len);
        res.bEnable = wifi_info.bEnable;
        
        res.HostIP= wifi_info.szHostIP;
        res.nStrength = wifi_info.nStrength;
        return true;
    }
    
    bool DhCamera::login(string ip_address="192.168.1.108", short port=37777, string user_name="admin", string passwd="admin123"){
        int error = 0;
        NET_DEVICEINFO_Ex deviceInfo;
        memset(&deviceInfo, 0, sizeof(NET_DEVICEINFO_Ex));
        _loginHandle = CLIENT_LoginEx2(ip_address.c_str(),port, user_name.c_str(), passwd.c_str(), EM_LOGIN_SPEC_CAP_TCP, NULL, &deviceInfo, &error);
        string errMsg;
        convertLoginError2String(error , errMsg);
        if(_loginHandle == 0)  {  // login failed.
            printf("Login failed! error:%s\n",errMsg.c_str());
            return false;
        }
        else  {  // login success.
            return true;
        }
    }

    void DhCamera::convertLoginError2String(int nErrorCode , string &strErrorCode){
        switch(nErrorCode) {
            case 0: 
                strErrorCode = "Login Success";
                break;
            case 1: 
                strErrorCode = "Account or Password Incorrect";
                break;
            case 2: 
                strErrorCode = "User Is Not Exist";
                break;
            case 3: 
                strErrorCode = "Login Timeout";
                break;
            case 4: 
                strErrorCode = "Repeat Login";
                break;
            case 5:
                strErrorCode = "User Account is Locked";
                break;
            case 6:
                strErrorCode = "User In Blacklist";
                break;
            case 7:
                strErrorCode = "Device Busy";
                break;
            case 8: 
                strErrorCode = "Sub Connect Failed";
                break;
            case 9:
                strErrorCode = "Host Connect Failed";
                break;
            case 10 :
                strErrorCode = "Max Connect";
                break;
            case 11:
                strErrorCode = "Support Protocol3 Only";
                break;
            case 12:
                strErrorCode = "UKey Info Error";
                break;
            case 13:
                strErrorCode = "No Authorized";
                break;
            case 18:
                strErrorCode = "Device Account isn't Initialized";
                break;
            default:
                strErrorCode = "Unknown Error";
                break; 
        }

    }

    void CALLBACK realDataCallBackEx(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LLONG param, LDWORD dwUser){
        DH_Channel_Info* pChannelInfo = (DH_Channel_Info*)dwUser;
        if(NULL == pChannelInfo){
            return;
        }
        // printf("stream byte size: %d\n",dwBufSize);
    }

    bool DhCamera::startWatching(){
        if(_loginHandle == 0)  {
            return false;
        }
        else{
            _playHandle = CLIENT_RealPlay(_loginHandle,0, NULL);
            if (_playHandle != 0){
                CLIENT_SetRealDataCallBackEx2(_playHandle, realDataCallBackEx, (LDWORD)&(_dwUser), 0x0f);
                return true;
            }
            else
            {
                printf("loginid = %ld;real play failed!\n", _loginHandle);
                return false;
            }

        }
    }

    bool DhCamera::stopWatching(){
        if( 0 == _playHandle){
            return true;
        }
        else{
            if(FALSE == CLIENT_StopRealPlayEx(_playHandle)) {
                return false;
            }
            else {
                _playHandle = 0;
                return true;
            }
        }
    }


    void DhCamera::cleanup() {
        logout();
        CLIENT_Cleanup();
    }

    void DhCamera::logout() {
        if (0 != _loginHandle){
            CLIENT_Logout(_loginHandle);
            _loginHandle = 0;
        }
    }

    void DhCamera::run(){
        if(!login()) 
            return;
        printf("login succeed!\n");
        _ptzCtrl.SetPtzParam(_loginHandle,0);
        ros::spin();
        logout();
        // if(startWatching()){
        //     printf("start watching!\n");
        //     while(true){
        //         std::cout<<"Input fastgo params: "<<std::endl;
        //         int nx,ny,nZoom;
        //         std::cin>>nx>>ny>>nZoom;
        //         _ptzCtrl.PtzFastGo(nx,ny,nZoom);
        //         // printf("Sleep!\n");
        //         // sleep(1);
        //     }
        // }else{
        //     printf("watching failed!\n");
        //     logout();
        // }

    }

}
