///*
// * movement.c
// *
// *  Created on: Jan 31, 2024
// *      Author: bsmith26
// */
//
//#include "movement.h"
//
//
//void move_forward(oi_t *sensor_data, double distance_mm) {
//    double sum = 0;
//    oi_setWheels(150, 150);
//
//    while (sum < distance_mm) {
//        oi_update(sensor_data);
//        sum += sensor_data->distance;
//        if(sensor_data -> bumpLeft) {
//            move_backward(sensor_data, 100);
//            turn_right(sensor_data, 90);
//            move_forward(sensor_data, 190);
//            turn_left(sensor_data, 90);
////            bumped = 1;
//            return;
//        }
//        else if (sensor_data -> bumpRight ) {
//            move_backward(sensor_data, 100);
//            turn_left(sensor_data, 90);
//            move_forward(sensor_data, 190);
//            turn_right(sensor_data, 90);
////            bumped = 1;
//            return;
//        }
//    }
//    oi_setWheels(0, 0);
////    bumped = 0;
//}
//
//void turn_left(oi_t *sensor, double degrees)
//{
//
//    double s = 0;
//    oi_setWheels(150, -150);
//
//    while (s < degrees)
//    {
//        oi_update(sensor);
//        s += sensor->angle;
//    }
//
//    oi_setWheels(0, 0);
//}
//
//void turn_right(oi_t *sensor, double degrees)
//{
//
//    double s = 0;
//    oi_setWheels(-150, 150);
//
//    while (s > -degrees)
//    {
//        oi_update(sensor);
//        s += sensor->angle;
//    }
//
//    oi_setWheels(0, 0);
//}
//
//void move_backward(oi_t *sensor_data, double distance_mm)
//{
//    double sum = 0;
//    oi_setWheels(-150, -150);
//
//    while (sum > -distance_mm)
//    {
//        oi_update(sensor_data);
//        sum += sensor_data->distance;
//    }
//
//    oi_setWheels(0, 0);
//}
//
//void turn(oi_t *sensor, double degrees)
//{
//    double angle_to_turn = (360.0 - turn_offset) * degrees / 360.0;
//
//    if (angle_to_turn > 90)
//    {
//        turn_left(sensor, angle_to_turn);
//    }
//    else
//    {
//        turn_right(sensor, angle_to_turn);
//    }
//}
//
//// void main() {
////    oi_t *sensor_data = oi_alloc();
////
////    oi_init(sensor_data);
////
////    double sum = 0;
////
////
////    while(1){
////        oi_setWheels(100, 100);
////        while(!(sensor_data -> bumpLeft) && !(sensor_data -> bumpRight) && sum < 2000) {
////            oi_update(sensor_data);
////            sum += sensor_data -> distance;
////        }
////        oi_setWheels(0,0);
////        if(sum >= 2000) {
////            break;
////        }
////        if(sensor_data -> bumpLeft) {
////            move_backward(sensor_data, 150);
////            sum -= 150;
////            turn_right(sensor_data, 90);
////            move_forward(sensor_data, 250);
////            turn_left(sensor_data, 90);
////        } else if(sensor_data -> bumpRight) {
////            move_backward(sensor_data, 150);
////            sum -= 150;
////            turn_left(sensor_data, 90);
////            move_forward(sensor_data, 250);
////            turn_right(sensor_data, 90);
////        }
////    }
////
////    oi_free(sensor_data);
////}
