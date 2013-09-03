#pragma once

#include <CentaurTypes.h>
#include <CentaurSockets.h>
#include <vector>
#include <string>
#include <iostream>
#include <libjson.h>
#include <stdint.h>

#include <chrono>
#include <thread>

#define HOKUYO_PORTNUMBER	"31777"
#define	RESPONSE_ERROR		"error"
#define RESPONSE_DATA		"data"
#define RESPONSE_TIMESTAMP	"timestamp"


struct HokuyoData {
	
	std::string						m_error;
	std::vector<std::vector<long> >	m_dataArrayArray;
	
	std::string toJSONString(){
		JSONNode retVal(JSON_NODE);
		retVal.push_back(JSONNode(RESPONSE_ERROR, m_error));
		JSONNode jsonArrayArray(JSON_ARRAY);
		jsonArrayArray.set_name(RESPONSE_DATA);
		for(unsigned int i = 0; i < m_dataArrayArray.size(); i++){
			std::vector<long> &data = m_dataArrayArray[i];
			JSONNode jsonArray(JSON_ARRAY);
			for(unsigned int j = 0; j < data.size(); j++)
				jsonArray.push_back(JSONNode("", data[j]));
			jsonArrayArray.push_back(jsonArray);
		}
		retVal.push_back(jsonArrayArray);
		return retVal.write();
	}

	void fromJSONString(const char * data){
		JSONNode jsonData = libjson::parse(data);
		JSONNode jsonError = jsonData.at(RESPONSE_ERROR);
		m_error = jsonError.as_string();
		JSONNode jsonArrayArray = jsonData.at(RESPONSE_DATA).as_array();
		JSONNode::iterator it = jsonArrayArray.begin();
		while(it != jsonArrayArray.end()){
			JSONNode jsonArray = it->as_array();
			JSONNode::iterator it2 = jsonArray.begin();
			std::vector<long> data;
			while(it2 != jsonArray.end()){
				data.push_back(it2->as_int());
				it2++;
			}
			m_dataArrayArray.push_back(data);
			it++;
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
		
		int64_t startTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);

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
		retVal.fromJSONString((const char *)response.getData());
		return retVal;
	}
	
};
