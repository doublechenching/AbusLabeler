#include "stdafx.h"
#include "MedicalVolume.h"

struct Position3D
{
	int x;
	int y;
	int z;
};


MedicalVolume::MedicalVolume(const char * filePath)
{
	m_label_buffer = NULL;
	m_pVoxelData = NULL;
	m_pDSCVoxelData = NULL;
	LoadDicomFile(filePath);
}


bool MedicalVolume::LoadDicomFile(const char* filePath)
{
	m_num_Tumors = 0;
	DcmMetaInfo * m_pMetaInfo;
	DcmDataset* m_pDataset;
	DcmFileFormat dicomFile;
	OFCondition status = dicomFile.loadFile(filePath);
	OFString strTmp;
	if (status.good())
	{
		m_pMetaInfo = dicomFile.getMetaInfo();
		m_pDataset = dicomFile.getDataset();
		// get height
		m_pDataset->findAndGetOFString(DCM_Rows, strTmp);
		m_heigth =  atoi(strTmp.c_str());
		// get width
		m_pDataset->findAndGetOFString(DCM_Columns, strTmp);
		m_width =  atoi(strTmp.c_str());
		// get depth
		m_pDataset->findAndGetOFString(DCM_NumberOfFrames, strTmp);
		m_depth =  atoi(strTmp.c_str());
		
		// get name
		m_pDataset->findAndGetOFString(DCM_PatientName, strTmp);
		m_patientName = strTmp.c_str();
		// get id
		strTmp.clear();
		m_pDataset->findAndGetOFString(DCM_PatientID, strTmp);
		m_patientID = strTmp.c_str();
		// get birthday
		m_pDataset->findAndGetOFString(DCM_PatientBirthDate, strTmp);
		m_birthday = CString(strTmp.c_str());
		// get spacing
		m_pDataset->findAndGetOFString(DCM_SpacingBetweenSlices, strTmp);
		m_slice_s =  atof(strTmp.c_str());

		m_pDataset->findAndGetOFString(DCM_PatientSex, strTmp);
		if (strcmp(strTmp.c_str(), "F"))
		{
			m_sex = FALSE;
		}
		else
		{
			m_sex = TRUE;
		}
		m_pDataset->findAndGetOFString(DCM_PatientAge, strTmp);
		m_age = atoi(strTmp.c_str());

		m_pDataset->findAndGetOFString(DCM_ViewName, strTmp);
		m_viewName = CString(strTmp.c_str());

		m_pDataset->findAndGetOFString(DCM_ViewName, strTmp);
		m_file_size = atoi(strTmp.c_str());

		m_pDataset->findAndGetOFString(DCM_PixelSpacing, strTmp, 0);
		m_height_s = atof(strTmp.c_str());

		m_pDataset->findAndGetOFString(DCM_PixelSpacing, strTmp, 1);
		m_width_s = atof(strTmp.c_str());

		// get pixel data
		const Uint8 * pUint8 = NULL;
		if (m_pVoxelData)
		{
			delete[] m_pVoxelData;
			delete[] m_pDSCVoxelData;
			m_pDSCVoxelData = NULL;
		}
		m_pVoxelData = new Uint8[m_heigth*m_width*m_depth];
		unsigned long count = 0;
		m_pDataset->findAndGetUint8Array(DCM_PixelData, pUint8, &count);
		memcpy(m_pVoxelData, pUint8, m_heigth*m_width*m_depth);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void MedicalVolume::GetAllInfo(DcmDataset* m_pDataset, vector<string> & pStrKeys, vector<string> & pStrValues)
{
	m_pDataset->print(std::cout);
	DcmObject* pDO = m_pDataset->nextInContainer(NULL);
	if (NULL == pDO)
	{
		return;
	}
	do
	{
		DcmTag dt = pDO->getTag();					//获取tag
		OFString value;
		
		DcmTagKey dtk = dt.getXTag();				//tag key即：（组号=dt.getGTag()，元素号=dt.getETag()）
		m_pDataset->findAndGetOFString(dtk, value); //根据tag key获取tag value
		pStrKeys.push_back(dt.getTagName());
		pStrValues.push_back(value.c_str());
		pDO = m_pDataset->nextInContainer(pDO);      //下一个DcmObject
	} while (pDO != NULL);
}


CString MedicalVolume::GetPatientName()
{
	return m_patientName;
}


CString MedicalVolume::GetPatientID()
{
	return m_patientID;
}


MedicalVolume::MedicalVolume()
{
	m_pVoxelData = NULL;
	m_pDSCVoxelData = NULL;
	m_label_buffer = NULL;
	m_dsc_borderpadding_len = 35;
	m_num_Tumors = 0;
}

CString MedicalVolume::GetPatientBirthDate()
{
	return m_birthday;
}

double MedicalVolume::GetSliceSpacing()
{
	return m_slice_s;
}

bool MedicalVolume::GetPatientSex()
{
	return m_sex;
}

int MedicalVolume::GetPatientAge()
{
	return m_age;
}

CString MedicalVolume::GetViewName()
{
	return m_viewName;
	
}

int MedicalVolume::GetFileSize()
{
	return m_file_size;
}

void MedicalVolume::GetPixelSpacing(double& s1, double& s2)
{
	s1 = m_height_s;
	s2 = m_width_s;
}

void MedicalVolume::GetViewImage(Uint8* imageBuffer, int viewNum, int x, int y, int z)
{
	int voxel_index = 0;
	int pixel_index = 0;
	int height = m_dsc_image_height;
	int width = m_dsc_image_width;
	int depth = m_depth;
	switch (viewNum)
	{
	case 1:
		// fix depth
		memcpy(imageBuffer, m_pDSCVoxelData + height * width * z, height * width);
		break;
	case 2:
		// fix width
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < depth; j++)
			{
				voxel_index = height * width * j + i * width + y;
				pixel_index = i * depth + j;
				imageBuffer[pixel_index] = m_pDSCVoxelData[voxel_index];
			}
		}
		break;
	case 3:
		// fix height
		for (int i = 0; i < depth; i++)
		{
			for (int j = 0; j < width; j++)
			{
				voxel_index = height * width * i + width * x + j;
				pixel_index = i * width + j;
				imageBuffer[pixel_index] = m_pDSCVoxelData[voxel_index];
			}
		}
		break;
	}
}


void MedicalVolume::GetViewLabel(Uint8* imageBuffer, int viewNum, int x, int y, int z)
{
	int voxel_index = 0;
	int pixel_index = 0;
	int height = m_dsc_image_height;
	int width = m_dsc_image_width;
	int depth = m_depth;
	switch (viewNum)
	{
	case 1:
		// fix depth
		memcpy(imageBuffer, m_label_buffer + height * width * z, height * width);
		break;
	case 2:
		// fix width
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < depth; j++)
			{
				voxel_index = height * width * j + i * width + y;
				pixel_index = i * depth + j;
				imageBuffer[pixel_index] = m_label_buffer[voxel_index];
			}
		}
		break;
	case 3:
		// fix height
		for (int i = 0; i < depth; i++)
		{
			for (int j = 0; j < width; j++)
			{
				voxel_index = height * width * i + width * x + j;
				pixel_index = i * width + j;
				imageBuffer[pixel_index] = m_label_buffer[voxel_index];
			}
		}
		break;
	}
}

int MedicalVolume::GetHeight()
{
	return m_heigth;
}

int MedicalVolume::GetWidth()
{
	return m_width;
}

int MedicalVolume::GetFrames()
{
	return m_depth;
}

Uint8* MedicalVolume::GetPixelData()
{
	return m_pVoxelData;
}

Uint8* MedicalVolume::CreatDSCBuffer()
{
	if (m_pDSCVoxelData)
	{
		delete[] m_pDSCVoxelData;
	}
	// 记住内存包括边界padding length
	m_dsc_image_height = m_heigth / m_width_s * m_height_s + m_dsc_borderpadding_len;
	m_dsc_image_width = m_width;
	int frame = m_depth;
	m_pDSCVoxelData = new Uint8[m_dsc_image_height*m_dsc_image_width*frame];
	memset(m_pDSCVoxelData, 0, m_dsc_image_height*m_dsc_image_width*frame);
	return m_pDSCVoxelData;
}

Uint8* MedicalVolume::GetDSCPixelData()
{
	return m_pDSCVoxelData;
}

int MedicalVolume::GetBorderPaddingLen()
{
	return m_dsc_borderpadding_len;
}

Uint8* MedicalVolume::CreatLabelBuffer()
{
	if (m_label_buffer)
	{
		delete[] m_label_buffer;
		m_label_buffer = NULL;
	}
	int size = GetFrames() * m_dsc_image_width * m_dsc_image_height;
	m_label_buffer = new Uint8[size];
	memset(m_label_buffer, 0, size);
	return m_label_buffer;
}

Uint8* MedicalVolume::GetLabelBuffer()
{
	return m_label_buffer;
}

Uint8* MedicalVolume::GetFirstFrame(Uint8* imageBuffer)
{
	int size = GetHeight() * GetWidth();
	memcpy(imageBuffer, m_pVoxelData, size);
	return imageBuffer;
}

MedicalVolume::~MedicalVolume()
{
	if (m_pVoxelData)
	{
		delete[] m_pVoxelData;
	}
	if (m_pDSCVoxelData)
	{
		delete[] m_pDSCVoxelData;
	}
	if (m_label_buffer)
	{
		delete[] m_label_buffer;
		m_label_buffer = NULL;
	}
}


