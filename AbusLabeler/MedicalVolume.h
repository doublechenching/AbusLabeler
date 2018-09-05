#pragma once
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

class MedicalVolume
{
public:
	MedicalVolume();
	MedicalVolume(const char* filePath);
	bool LoadDicomFile(const char* filePath);
	void GetAllInfo(DcmDataset* m_pDataset, vector<string> & pStrKeys, vector<string> & pStrValues);
	CString GetPatientName();
	CString GetPatientID();
	CString GetPatientBirthDate();
	double GetSliceSpacing();
	bool GetPatientSex();
	int GetPatientAge();
	CString GetViewName();
	int GetFileSize();
	void GetPixelSpacing(double& s1, double& s2);
	void GetNipplePos(double &x, double& y, double& z);
	void GetViewImage(Uint8* imageBuffer, int viewNum, int x, int y, int z);
	void GetViewLabel(Uint8* imageBuffer, int viewNum, int x, int y, int z);
	int GetHeight();
	int GetWidth();
	int GetFrames();
	Uint8* GetPixelData();
	Uint8* CreatDSCBuffer();
	Uint8* GetDSCPixelData();
	int GetBorderPaddingLen();
	Uint8* CreatLabelBuffer();
	Uint8* GetLabelBuffer();
	Uint8* GetFirstFrame(Uint8* imageBuffer);
	~MedicalVolume();

private:
	Uint8* m_pVoxelData;
	Uint8* m_pDSCVoxelData;
	int m_heigth;
	int m_width;
	int m_depth;
	CString m_patientName;
	CString m_patientID;
	CString m_birthday;
	double m_slice_s;
	double m_height_s;
	double m_width_s;
	bool m_sex;
	int m_age;
	CString m_viewName;
	int m_file_size;
	int m_dsc_borderpadding_len;
	double m_nipple_pos_x;
	double m_nipple_pos_y;
	double m_nipple_pos_z;
	Uint8* m_label_buffer;
public:
	int m_dsc_image_height;
	int m_dsc_image_width;
	int m_num_Tumors;
};



