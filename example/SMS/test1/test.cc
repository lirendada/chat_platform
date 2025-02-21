#include <cstdlib>
#include <iostream>
#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/core/CommonRequest.h>
#include <alibabacloud/core/CommonClient.h>
#include <alibabacloud/core/CommonResponse.h>

using namespace std;
using namespace AlibabaCloud;

int main( int argc, char** argv )
{
    AlibabaCloud::InitializeSdk();
    AlibabaCloud::ClientConfiguration configuration( "cn-chengdu" );
    configuration.setConnectTimeout(1500);
    configuration.setReadTimeout(4000);

    // 需要关注两个key：
    std::string access_key_id = "LTAI5t9kmjtWSZQ4mvqTaW9g";
    std::string access_key_secret = "VYkXgQO679Azxq7EY4n4hNcOQeobqN";
    AlibabaCloud::Credentials credential(access_key_id, access_key_secret);
    
    AlibabaCloud::CommonClient client( credential, configuration );
    AlibabaCloud::CommonRequest request(AlibabaCloud::CommonRequest::RequestPattern::RpcPattern);
    request.setHttpMethod(AlibabaCloud::HttpRequest::Method::Post);
    request.setDomain("dysmsapi.aliyuncs.com");
    request.setVersion("2017-05-25");
    request.setQueryParameter("Action", "SendSms");
    request.setQueryParameter("SignName", "利刃大大");
    request.setQueryParameter("TemplateCode", "SMS_479250309");
    
    // 然后关注下面的手机号以及验证码即可：
    request.setQueryParameter("PhoneNumbers", "18063551124");
    request.setQueryParameter("TemplateParam", "{\"code\":\"1234\"}");

    auto response = client.commonResponse(request);
    if (response.isSuccess()) {
        printf("request success.\n");
        printf("result: %s\n", response.result().payload().c_str());
    } else {
        printf("error: %s\n", response.error().errorMessage().c_str());
        printf("request id: %s\n", response.error().requestId().c_str());
    }

    AlibabaCloud::ShutdownSdk();
    return 0;
}