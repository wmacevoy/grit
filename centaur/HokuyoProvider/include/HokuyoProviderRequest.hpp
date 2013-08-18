#pragma once

#include <CentaurTypes.h>
#include <CentaurSockets.h>
#include <bson.h>
#include <vector>
#include <string>
#include <iostream>

#define HOKUYO_PORTNUMBER	"31777"
#define	RESPONSE_ERROR		"error"
#define RESPONSE_DATA		"data"
#define RESPONSE_TIMESTAMP	"timestamp"


struct HokuyoData {
	
	std::string						m_error;
	double							m_timestamp;
	std::vector<std::vector<long> >	m_dataArrayArray;
	
	void printToStdOut(){
		if(m_error.empty()) {
			std::cout << "Printing data\n";
			for(int i = 0; i < m_dataArrayArray.size(); i++){
				std::vector<long> &pointData = m_dataArrayArray[i];
				
				for(int j = 0; j < pointData.size(); j++) {
					std::cout << pointData[j];
					if(j < pointData.size())
						std::cout << ", ";
				}
				std::cout << "\n";
			}
		} else {
			
			std::cout << "ERROR:\n\t"<< m_error << "\n";
		}
	}
	
	bson::bo toBSON(){
		bson::BSONObjBuilder objBuilder;
		bson::BSONArrayBuilder arrayBuilder;
		if(m_error.empty()){
			for(int i = 0; i < m_dataArrayArray.size(); i++){
				vector<long> &data = m_dataArrayArray[i];
				bson::BSONArrayBuilder dataBuilder;
				for(int j = 0; j < data.size(); j++)
					dataBuilder.append((long long)data[j]);
				arrayBuilder.append(dataBuilder.arr());
			}
			objBuilder.append(RESPONSE_DATA, arrayBuilder.arr());
			objBuilder.append(RESPONSE_ERROR, "");
		} else {
			objBuilder.append(RESPONSE_DATA, arrayBuilder.arr());
			objBuilder.append(RESPONSE_ERROR, m_error);
		}
		objBuilder.append(RESPONSE_TIMESTAMP, m_timestamp);
		return objBuilder.obj();
	}
	
	void fromBSON(const char * input){
		bson::bo responseObj(input);
		m_timestamp = responseObj.getField(RESPONSE_TIMESTAMP).Double();
		m_error = responseObj.getField(RESPONSE_ERROR).String();
		if(!m_error.empty())
			return;
		
		std::vector<bson::BSONElement> bDataArrayArray = responseObj.getField(RESPONSE_DATA).Array();
		for(int i = 0; i < bDataArrayArray.size(); i++){
			std::vector<bson::BSONElement> bDataObj = bDataArrayArray[i].Array();
			std::vector<long> data;
			for(int j = 0; j < bDataObj.size(); j++){
				data.push_back(bDataObj[j].numberLong());
			}
			m_dataArrayArray.push_back(data);
		}
	}
	
};

class HokuyoProviderRequest {
	
public:

	static HokuyoData GetData(const unsigned int nScans) {
		HokuyoData retVal;
	
		CentaurSocketReq request;
		std::string tmp("tcp://192.168.2.100:"); //tcp://192.168.2.100:
		tmp += HOKUYO_PORTNUMBER;
		if(!request.open(tmp.c_str())){
			retVal.m_error = "Unable to connect to zmq port ";
			retVal.m_error += tmp;
			retVal.m_error += "\n";
			return retVal;		
		}
		
		if(!request.send(&nScans, sizeof(nScans))){
			retVal.m_error = "Unable to send request\n";
			return retVal;
		}
		
		CM_Array<char, 2048> response;
		if(!request.recv(response, true)){
			retVal.m_error = "Unable to receive data\n";
			return retVal;
		}
		retVal.fromBSON((const char *)response.getData());
		return retVal;
	}
	
};
