#pragma once

class MP3
{
public:
	MP3();
	virtual ~MP3();
	void Init();
	void PlayFileSync(const char *FileName);

private:
	void Mp3WriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte);
	unsigned int Mp3ReadRegister(unsigned char addressbyte); 
	void Mp3SetVolume(unsigned char leftchannel, unsigned char rightchannel);

};