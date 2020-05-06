#!/usr/bin/python
import cv2
import rospy
from alhua_ptz_camera.srv import PtzCtrl

def draw_cross(frame):
    middle_point = (1920/2,1080/2)
    half_length = 20
    thickness = 5
    color = (0, 0, 255) 
    cv2.line(frame,(middle_point[0]-half_length,middle_point[1]) ,(middle_point[0]+half_length,middle_point[1]) ,color,thickness)
    cv2.line(frame,(middle_point[0],middle_point[1]-half_length) ,(middle_point[0],middle_point[1]+half_length) ,color,thickness)
    

def fast_go(event,x,y,flags,param):
    if event == cv2.EVENT_LBUTTONDOWN:
        print "x:%d,y:%d"%(x,y)
        middle_point = (1920/2,1080/2)
        dis_x,dis_y = x-middle_point[0],y-middle_point[1]
        print "dis_x:%d,dis_y:%d"%(dis_x,dis_y)
        param_1,param_2 = float(dis_x)/middle_point[0]*8192,float(dis_y)/middle_point[1]*8192
        print "param_1:%d,param_2:%d"%(param_1,param_2)
        rospy.wait_for_service('ptz_ctrl')
        try:
            ptz_ctrl = rospy.ServiceProxy('ptz_ctrl', PtzCtrl)
            resp1 = ptz_ctrl(1,int(param_1),int(param_2),0)
            print resp1.status_message
        except rospy.ServiceException, e:
            print "Service call failed: %s"%e

if __name__ == "__main__":
    rospy.init_node("fast_go_test")
    vcap = cv2.VideoCapture("rtsp://admin:admin123@192.168.1.108:554/cam/realmonitor?channel=1&subtype=0")
    cv2.namedWindow('VIDEO')
    cv2.setMouseCallback('VIDEO', fast_go)
    while(not rospy.is_shutdown()):
        ret, frame = vcap.read()
        draw_cross(frame)
        cv2.imshow('VIDEO', frame)
        if cv2.waitKey(20) & 0xFF == 27:
            break
    cv2.destroyAllWindows()
