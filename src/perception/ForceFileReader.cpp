/*
 * ForceFileReader.cpp
 *
 *  Created on: 27/06/2017
 *      Author: blackzafiro
 */

#include "perception/ForceFileReader.h"

namespace Perception {

	///
	/// SensedForce
	///

	SensedForce::SensedForce() : fx(0), fy(0), fz(0), tx(0), ty(0), tz(0), d(0) {}

	std::ostream& operator << (std::ostream &o, const SensedForce& f){
		return o << std::fixed << std::setprecision(3) << f.fx << "\t"
				<< f.fy << "\t" << f.fz << "\t" << f.tx << "\t" << f.ty << "\t" << f.tz << "\t" << f.d;
	}



	///
	/// SensorFileReader
	///

	int SensorFileReader::readLine(SensedForce &sf, float &time){
		if(sensorFile.is_open() &&  sensorFile.good() && sensorFile.peek() != -1){ // !sensorFile.eof() is included in good
			//std::cout << "eof:" << sensorFile.eof() << " nextchar:" << sensorFile.peek() << std::endl;

			sensorFile >> time;
			//char tab;
			//sensorFile >> tab;
			sensorFile >> sf.fx;
			//sensorFile >> coma;
			sensorFile >> sf.fy;
			//sensorFile >> coma;
			sensorFile >> sf.fz;

			//sensorFile >> coma;
			sensorFile >> sf.tx;
			//sensorFile >> coma;
			sensorFile >> sf.ty;
			//sensorFile >> coma;
			sensorFile >> sf.tz;
			sensorFile >> sf.d;

			std::getline(sensorFile, line);
			//std::cout << "time:" << time  << " possition y:" << force.effector_y <<
			//             " torque y:" << force.torque_y << "\t" << line << std::endl;

			//std::cout << line << std::endl;
			//std::cout << "eof:" << sensorFile.eof() << std::endl;
			return 1;
		}else return -1;
	}

	SensorFileReader::SensorFileReader() : time(0), prevTime(-1) { }

	int SensorFileReader::init (const char* s){
		sensorFile.open(s);
		if(sensorFile.is_open()){
			return 0;
		}
		else return -1;
	}

	int SensorFileReader::readLine(){
		prevTime = time;
		prevForce = force;
		return readLine(force, time);
	}

	float SensorFileReader::getTimestamp(){
		return time;
	}

	SensedForce& SensorFileReader::getForce(){
		return force;
	}

	float SensorFileReader::getPrevTimestamp(){
		return prevTime;
	}

	SensedForce& SensorFileReader::getPrevForce(){
		return prevForce;
	}

	void SensorFileReader::close(){
		if(sensorFile.is_open()) sensorFile.close();
	}

}
