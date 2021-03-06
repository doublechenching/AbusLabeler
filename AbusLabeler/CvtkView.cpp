// CvtkView.cpp: 实现文件
//

#include "stdafx.h"
#include "AbusLabelerDlg.h"
#include "CvtkView.h"
#include "vtkRenderWindowInteractor.h"
// CvtkView

IMPLEMENT_DYNAMIC(CvtkView, CStatic)

CvtkView::CvtkView()
{

}

CvtkView::~CvtkView()
{
}


BEGIN_MESSAGE_MAP(CvtkView, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CvtkView 消息处理程序




void CvtkView::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	// 实例化和构建可视化管线
	CRect rect;
	GetClientRect(rect);
	// 初始化renderer
	m_Renderer = vtkSmartPointer<vtkRenderer>::New();
	m_RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	m_RenderWindow->SetParentId(this->m_hWnd);
	m_RenderWindow->SetSize(rect.Width(), rect.Height());
	m_RenderWindow->AddRenderer(m_Renderer);
	// 初始化渲染窗口
	if (m_RenderWindow->GetInteractor() == NULL)
	{
		vtkSmartPointer<vtkRenderWindowInteractor> RenderWindowInteractor =
			vtkSmartPointer<vtkRenderWindowInteractor>::New();
		RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
		RenderWindowInteractor->Initialize();
	}
	m_ImagePlaneWidget = vtkSmartPointer< vtkImagePlaneWidget >::New();
	m_ResliceCursor = vtkSmartPointer< vtkResliceCursor >::New();
	m_ResliceCursorRep = vtkSmartPointer< vtkResliceCursorThickLineRepresentation >::New();
	m_ResliceCursorRep->GetResliceCursorActor()->
		GetCursorAlgorithm()->SetResliceCursor(m_ResliceCursor);
	// 初始化重切光标工具
	m_ResliceCursorWidget = vtkSmartPointer< vtkResliceCursorWidget>::New();
	m_ResliceCursorWidget->SetInteractor(m_RenderWindow->GetInteractor());
	m_ResliceCursorWidget->SetDefaultRenderer(m_Renderer);
	m_ResliceCursorWidget->SetRepresentation(m_ResliceCursorRep);

	m_RenderWindow->Start();

	CStatic::PreSubclassWindow();
}

void CvtkView::SetImageData(vtkSmartPointer<vtkImageData> ImageData)
{
	// 当新数据传进来时，初始化
	if (ImageData == NULL) return;	// 如果数据不存在退出
	m_ImageData = ImageData;
	SetupReslice();
	Render();
}

void CvtkView::SetupReslice()
{
	if (m_ImageData == NULL) return;
	int dims[3];
	m_ImageData->GetDimensions(dims);
	// 建立流图
	m_ImagePlaneWidget->SetInputData(m_ImageData);			// 设置输入图像
	m_ImagePlaneWidget->SetPlaneOrientation(m_Direction);	// 设置切片的方向
	m_ImagePlaneWidget->SetSliceIndex(dims[m_Direction] / 2);	// 设置当前方向上默认的层号
	m_ImagePlaneWidget->On();
	m_ImagePlaneWidget->InteractionOn();

	m_ResliceCursor->SetCenter(m_ImageData->GetCenter());	// 设置输入图像
	m_ResliceCursor->SetImage(m_ImageData);					// 设置默认的切分中心点
	m_ResliceCursor->SetThickMode(0);						// 设置切分模式
	// 参数为0时每次切分得到一个单层的图像切片；而当参数为1时开启厚度模式
	m_ResliceCursorRep->GetResliceCursorActor()->
		GetCursorAlgorithm()->SetResliceCursor(m_ResliceCursor);
	m_ResliceCursorRep->GetResliceCursorActor()->
		GetCursorAlgorithm()->SetReslicePlaneNormal(m_Direction);

	m_ResliceCursorWidget->SetEnabled(1);
	m_Renderer->ResetCamera();

	//////////////////////////////////////////////////////////////////////////
	double range[2];
	m_ImageData->GetScalarRange(range);
	m_ResliceCursorWidget->GetResliceCursorRepresentation()->
		SetWindowLevel(range[1] - range[0], (range[0] + range[1]) / 2.0);
	m_ImagePlaneWidget->SetWindowLevel(range[1] - range[0], (range[0] + range[1]) / 2.0);
}

void CvtkView::Render()
{
	m_RenderWindow->Render();
}

void CvtkView::SetResliceCursor(vtkSmartPointer< vtkResliceCursor > cursor)
{
	m_ResliceCursor = cursor;
}


void CvtkView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CStatic::OnPaint()
}
