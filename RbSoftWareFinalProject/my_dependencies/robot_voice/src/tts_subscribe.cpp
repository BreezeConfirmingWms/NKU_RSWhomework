/*
* 语音合成（Text To Speech，TTS）技术能够自动将任意文字实时转换为连续的
* 自然语音，是一种能够在任何时间、任何地点，向任何人提供语音信息服务的
* 高效便捷手段，非常符合信息时代海量数据、动态更新和个性化查询的需求。
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include<locale>
#include<codecvt>
#include <boost/locale/encoding_utf.hpp>

#include "robot_voice/qtts.h"
#include "robot_voice/msp_cmn.h"
#include "robot_voice/msp_errors.h"

#include "ros/ros.h"
#include "std_msgs/String.h"


 #include <sstream>
 #include <sys/types.h>
 #include <sys/stat.h>


#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/translit.h>
 
using boost::locale::conv::utf_to_utf;

/* wav音频头部格式 */
typedef struct _wave_pcm_hdr
{
	char            riff[4];                // = "RIFF"
	int		size_8;                 // = FileSize - 8
	char            wave[4];                // = "WAVE"
	char            fmt[4];                 // = "fmt "
	int		fmt_size;		// = 下一个结构体的大小 : 16

	short int       format_tag;             // = PCM : 1
	short int       channels;               // = 通道数 : 1
	int		samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
	int		avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
	short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
	short int       bits_per_sample;        // = 量化比特数: 8 | 16

	char            data[4];                // = "data";
	int		data_size;              // = 纯数据长度 : FileSize - 44 
} wave_pcm_hdr;

/* 默认wav音频头部数据 */
wave_pcm_hdr default_wav_hdr = 
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{'d', 'a', 't', 'a'},
	0  
};

std::wstring binaryTostring(const std::string& binaryString){
	std::string result;

	// for(size_t i=2;i<binaryString.length()-1;i++){
	// 	unsigned char byte = static_cast<unsigned char>(binaryString[i]);

	// 	result+=byte;
	// }

	std :: string  utf8String = binaryString.substr(2,binaryString.length()-3);

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring wideString = converter.from_bytes(binaryString);
	return wideString;
	
}


// std::string utf8_to_string(const std::string& utf8String) {
//     UErrorCode error = U_ZERO_ERROR;
//     UConverter* converter = ucnv_open("UTF-8", &error);

//     const char* src = utf8String.c_str();
//     int32_t srcLength = static_cast<int32_t>(utf8String.length());
//     int32_t destCapacity = ucnv_getMaxCharSize(converter) * srcLength;

//     UChar* dest = new UChar*[destCapacity];
//     int32_t destLength = ucnv_toUChars(converter, dest, destCapacity, src, srcLength, &error);

//     std::string result(dest, destLength);

//     delete[] dest;
//     ucnv_close(converter);

//     return result;
// }


std::wstring utf8_to_wstring(const std::string& str)
{
    return utf_to_utf<wchar_t>(str.c_str(), str.c_str() + str.size());
}

std::string wstring_to_utf8(const std::wstring& str)
{
    return utf_to_utf<char>(str.c_str(), str.c_str() + str.size());
}  
/* 文本合成 */
int text_to_speech(const char* src_text, const char* des_path, const char* params)
{
	int          ret          = -1;
	FILE*        fp           = NULL;
	const char*  sessionID    = NULL;
	unsigned int audio_len    = 0;
	wave_pcm_hdr wav_hdr      = default_wav_hdr;
	int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

	if (NULL == src_text || NULL == des_path)
	{
		printf("params is error!\n");
		return ret;
	}
	fp = fopen(des_path, "wb");
	if (NULL == fp)
	{
		printf("open %s error.\n", des_path);
		return ret;
	}
	/* 开始合成 */
	sessionID = QTTSSessionBegin(params, &ret);
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSSessionBegin failed, error code: %d.\n", ret);
		fclose(fp);
		return ret;
	}
	ret = QTTSTextPut(sessionID, src_text, (unsigned int)strlen(src_text), NULL);
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSTextPut failed, error code: %d.\n",ret);
		QTTSSessionEnd(sessionID, "TextPutError");
		fclose(fp);
		return ret;
	}
	printf("正在合成 ...\n");
	fwrite(&wav_hdr, sizeof(wav_hdr) ,1, fp); //添加wav音频头，使用采样率为16000
	while (1) 
	{
		/* 获取合成音频 */
		const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if (MSP_SUCCESS != ret)
			break;
		if (NULL != data)
		{
			fwrite(data, audio_len, 1, fp);
		    wav_hdr.data_size += audio_len; //计算data_size大小
		}
		if (MSP_TTS_FLAG_DATA_END == synth_status)
			break;
		printf(">");
		usleep(150*1000); //防止频繁占用CPU
	}
	printf("\n");
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSAudioGet failed, error code: %d.\n",ret);
		QTTSSessionEnd(sessionID, "AudioGetError");
		fclose(fp);
		return ret;
	}
	/* 修正wav文件头数据的大小 */
	wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);
	
	/* 将修正过的数据写回文件头部,音频文件为wav格式 */
	fseek(fp, 4, 0);
	fwrite(&wav_hdr.size_8,sizeof(wav_hdr.size_8), 1, fp); //写入size_8的值
	fseek(fp, 40, 0); //将文件指针偏移到存储data_size值的位置
	fwrite(&wav_hdr.data_size,sizeof(wav_hdr.data_size), 1, fp); //写入data_size的值
	fclose(fp);
	fp = NULL;
	/* 合成完毕 */
	ret = QTTSSessionEnd(sessionID, "Normal");
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSSessionEnd failed, error code: %d.\n",ret);
	}

	return ret;
}
std::string bytesToUtf8(const std::string& bytes) {
    return std::string(bytes.begin(), bytes.end());
}


std::wstring utf8ToWstring(const std::string& utf8String) {
    UErrorCode errorCode = U_ZERO_ERROR;
    UConverter* utf8Converter = ucnv_open("UTF-8", &errorCode);
    if (U_SUCCESS(errorCode)) {
        const char* utf8Data = utf8String.c_str();
        int32_t utf8Length = utf8String.length();
        
        UChar* unicodeBuffer = new UChar[utf8Length + 1];
        int32_t unicodeLength = ucnv_toUChars(utf8Converter, unicodeBuffer, utf8Length + 1, utf8Data, utf8Length, &errorCode);
        
        if (U_SUCCESS(errorCode)) {
            std::wstring wstr(unicodeBuffer, unicodeBuffer + unicodeLength);
            
            delete[] unicodeBuffer;
            ucnv_close(utf8Converter);
            
            return wstr;
        }
    }
    
    ucnv_close(utf8Converter);
    return L"";
}
void voiceWordsCallback(const std_msgs::String::ConstPtr& msg)
{
    char cmd[2000];
    const char* text;
    int         ret                  = MSP_SUCCESS;
    const char* session_begin_params = "voice_name = xiaoyan, text_encoding = utf8, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
    const char* filename             = "tts_sample.wav"; //合成的语音文件名称


    
	int len = msg->data.length();

	
	// const char*  msgData = msg->data.substr(2,msg->data.length()-3).c_str();

	// std::cout<<"received:"<<msgData<<std::endl;


	// icu_68::Transliterator* transliterator = icu_68::Transliterator::createInstance("Hex-Any");  // 创建转换器实例

    // icu_68::UnicodeString output;
    

	// icu_68::UnicodeString input(msgData);

	// transliterator->transliterate(input, output); 

	// std::string result;
    // output.toUTF8String(result); 

	// std::string utf8String = "\xE4\xBD\xA0\xE5\xA5\xBD";  // UTF-8编码的字符串

    // std::wstring wstr = utf8ToWstring(utf8String);
    // std::wcout << "转换结果: " << wstr << std::endl;

	//std::string msgFinal ="\xe4\xbd\xa0\xe5\xa5\xbd";

	// for(int i=2;i<msg->data.length()-1;i++)
	// 	msgFinal += msg->data[i];

	//ROS_INFO("received:%s",wstr);

	
    // std::wstring wstrs = L"你好";  // 宽字符字符串

    // std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    // std::string str = converter.to_bytes(wstrs);  // 将宽字符字符串转换为窄字符字符串
    

	 text  = msg->data.c_str();

	// text =  bytesToUtf8(msg->data).c_str();
	std::cout<<"I heard :"<<text<<std::endl;
	//ROS_INFO(msg->data.c_str());

    /* 文本合成 */
    printf("开始合成 ...\n");
    ret = text_to_speech(text, filename, session_begin_params);
    if (MSP_SUCCESS != ret)
    {
        printf("text_to_speech failed, error code: %d.\n", ret);
    }
    printf("合成完毕\n");

	unlink("/tmp/cmd");  
    mkfifo("/tmp/cmd", 0777);  
    popen("mplayer -quiet -slave -input file=/tmp/cmd 'tts_sample.wav'","r");
 
    //popen("cvlc tts_sample.wav","r");
    sleep(1);
}

void toExit()
{
    printf("按任意键退出 ...\n");
    getchar();
    MSPLogout(); //退出登录
}

int main(int argc, char* argv[])
{
	int         ret                  = MSP_SUCCESS;
	const char* login_params         = "appid = 0032e1af, work_dir = .";//登录参数,appid与msc库绑定,请勿随意改动
	/*
	* rdn:           合成音频数字发音方式
	* volume:        合成音频的音量
	* pitch:         合成音频的音调
	* speed:         合成音频对应的语速
	* voice_name:    合成发音人
	* sample_rate:   合成音频采样率
	* text_encoding: 合成文本编码格式
	*
	*/
	//const char* session_begin_params = "voice_name = xiaoyan, text_encoding = utf8, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
	//const char* filename             = "tts_sample.wav"; //合成的语音文件名称
	//const char* text                 = "亲爱的用户，您好，这是一个语音合成示例，感谢您对科大讯飞语音技术的支持！科大讯飞是亚太地区最大的语音上市公司，股票代码：002230"; //合成文本

	/* 用户登录 */
	ret = MSPLogin(NULL, NULL, login_params);//第一个参数是用户名，第二个参数是密码，第三个参数是登录参数，用户名和密码可在http://www.xfyun.cn注册获取
	if (MSP_SUCCESS != ret)
	{
		printf("MSPLogin failed, error code: %d.\n", ret);
		//goto exit ;//登录失败，退出登录
        toExit();
	}
	printf("\n###########################################################################\n");
	printf("## 语音合成（Text To Speech，TTS）技术能够自动将任意文字实时转换为连续的 ##\n");
	printf("## 自然语音，是一种能够在任何时间、任何地点，向任何人提供语音信息服务的  ##\n");
	printf("## 高效便捷手段，非常符合信息时代海量数据、动态更新和个性化查询的需求。  ##\n");
	printf("###########################################################################\n\n");

    ros::init(argc,argv,"TextToSpeech");
    ros::NodeHandle n;
    ros::Subscriber sub =n.subscribe("voiceWords", 1000, voiceWordsCallback);
    ros::spin();

exit:
	printf("按任意键退出 ...\n");
	getchar();
	MSPLogout(); //退出登录

	return 0;
}

