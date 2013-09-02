#pragma once

#include <CentaurTypes.h>
#include <CentaurSockets.h>
#include <bson.h>
#include <vector>
#include <string>
#include <iostream>

#include <chrono>
#include <thread>

#define HOKUYO_PORTNUMBER	"31777"
#define	RESPONSE_ERROR		"error"
#define RESPONSE_DATA		"data"
#define RESPONSE_TIMESTAMP	"timestamp"


struct HokuyoData {
	
	std::string						m_error;
	std::vector<std::vector<long> >	m_dataArrayArray;
	
	void printToStdOut(){
		if(m_error.empty()) {
			std::cout << "Printing data\n";
			for(unsigned int i = 0; i < m_dataArrayArray.size(); i++){
				std::vector<long> &pointData = m_dataArrayArray[i];
				
				for(unsigned int j = 0; j < pointData.size(); j++) {
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
			for(unsigned int i = 0; i < m_dataArrayArray.size(); i++){
				vector<long> &data = m_dataArrayArray[i];
				bson::BSONArrayBuilder dataBuilder;
				for(unsigned int j = 0; j < data.size(); j++)
					dataBuilder.append((long long)data[j]);
				arrayBuilder.append(dataBuilder.arr());
			}
			objBuilder.append(RESPONSE_DATA, arrayBuilder.arr());
			objBuilder.append(RESPONSE_ERROR, "");
		} else {
			objBuilder.append(RESPONSE_DATA, arrayBuilder.arr());
			objBuilder.append(RESPONSE_ERROR, m_error);
		}
		return objBuilder.obj();
	}
	
	void fromBSON(const char * input){
		bson::bo responseObj(input);
		m_error = responseObj.getField(RESPONSE_ERROR).String();
		if(!m_error.empty())
			return;
		
		std::vector<bson::BSONElement> bDataArrayArray = responseObj.getField(RESPONSE_DATA).Array();
		for(unsigned int i = 0; i < bDataArrayArray.size(); i++){
			std::vector<bson::BSONElement> bDataObj = bDataArrayArray[i].Array();
			std::vector<long> data;
			for(unsigned int j = 0; j < bDataObj.size(); j++){
				data.push_back((long)bDataObj[j].numberLong());
			}
			m_dataArrayArray.push_back(data);
		}
	}
	
};

class HokuyoProviderRequest {
	
public:

	static HokuyoData GetData(const char * providerAddress, const unsigned int nScans) {
		
		HokuyoData retVal;
	
		CentaurSocketReq request;
		std::string tmp;
		tmp  = providerAddress;
		tmp += ":";
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
		
		__int64 startTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);

		int bytesReceived;

		CM_Array<char, 2048> response;
		while (		(bytesReceived = request.recv(response, false)) < 0
				&&	request.getError() == EAGAIN
				&&	(std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1)) - startTime < 50000)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		if (bytesReceived < 0)
		{
			retVal.m_error = "Unable to receive data\n";
			return retVal;
		}
		retVal.fromBSON((const char *)response.getData());
		return retVal;
	}
	
};
