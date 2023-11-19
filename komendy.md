roslaunch stero_mobile_init tiago_simple.launch world:=korytarz

rosrun teleop_twist_keyboard teleop_twist_keyboard.py cmd_vel:=key_vel

rosrun gmapping slam_gmapping scan:=scan_raw

rosrun map_server map_saver

rosrun tf static_transform_publisher 0 0 0 0 0 0 map odom 100