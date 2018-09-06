//
//
//  jupiter
//
//  Created by caik on 2017/7/14.
//  Copyright © 2017年 wedoctor. All rights reserved.
//


#ifndef SRC_SHORTLINK_PACKER_H_
#define SRC_SHORTLINK_PACKER_H_

#include <string>
#include <map>

class AutoBuffer;

namespace mars { namespace stn {

class shortlink_tracker {
public:
    static shortlink_tracker* (*Create)();
    
public:
    virtual ~shortlink_tracker(){}
};
//NOLINTNEXTLINE
extern void (*shortlink_pack)(const std::string& _url, const std::map<std::string, std::string>& _headers, const AutoBuffer& _body, const AutoBuffer& _extension, AutoBuffer& _out_buff, shortlink_tracker* _tracker);

}// namespace stn
}// namespace mars

#endif /* SRC_SHORTLINK_PACKER_H_ */
