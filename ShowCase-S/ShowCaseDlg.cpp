
// ShowCaseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ShowCase.h"
#include "ShowCaseDlg.h"
#include "afxdialogex.h"
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CShowCaseDlg 对话框



CShowCaseDlg::CShowCaseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowCaseDlg::IDD, pParent)
	, m_currentNodeId(-1)
	, m_currentNodeEnergy(0)
	, m_curerntNodeWorkingTime(0)
	, m_currentWorkType(_T("Receiving"))
	, m_currentNodeRelaxedTime(0)
	, m_currentNodeStatus(_T("NonGate"))
	, m_currentTime(0)
	, m_currentNodeTotalWorkTime(0)
	, m_allNeighbors(_T(""))
	, m_flag(false)
	, nt(NULL)
	, m_po(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShowCaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ID_STATIC, m_currentNodeId);
	DDX_Text(pDX, IDC_ENERGY_STATIC, m_currentNodeEnergy);
	DDX_Text(pDX, IDC_WORKING_TIME_STATIC, m_curerntNodeWorkingTime);
	DDX_Text(pDX, IDC_WORK_TYPE_STATIC, m_currentWorkType);
	DDX_Text(pDX, IDC_RELAXED_TIME_STATIC, m_currentNodeRelaxedTime);
	DDX_Text(pDX, IDC_STATUS_STATIC, m_currentNodeStatus);
	DDX_Control(pDX, IDC_TIME_SPIN, m_timeSpinCtrl);
	DDX_Text(pDX, IDC_TIME_EDIT, m_currentTime);
	DDX_Control(pDX, IDC_PIC_STATIC, m_pic);
	DDX_Text(pDX, IDC_TOTAL_TIME_STATIC, m_currentNodeTotalWorkTime);
	DDX_Text(pDX, IDC_NEIGHBERS_EDIT, m_allNeighbors);
}

BEGIN_MESSAGE_MAP(CShowCaseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_TIME_EDIT, &CShowCaseDlg::OnEnChangeTimeEdit)
	ON_STN_CLICKED(IDC_PIC_STATIC, &CShowCaseDlg::OnStnClickedPicStatic)
	ON_WM_TIMER()
	ON_STN_DBLCLK(IDC_PIC_STATIC, &CShowCaseDlg::OnStnDblclickPicStatic)
	ON_BN_CLICKED(IDC_RUNBTN, &CShowCaseDlg::OnBnClickedRunbtn)
END_MESSAGE_MAP()


// CShowCaseDlg 消息处理程序

BOOL CShowCaseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//CRect temprect(0,0,1640,1480);
  // SetWindowPos(NULL,0,0,temprect.Width(),temprect.Height(),SWP_NOZORDER|SWP_NOMOVE);
//	SetWindowPos(NULL, 0, 0, 1000, 500, SWP_SHOWWINDOW );
	CaculateCurrent();
	CenterWindow();
	GetClientRect(&m_rect);//得到当前对话框的大小
	m_timeSpinCtrl.SetRange(0, SIMULATION_TIME);
	m_timeSpinCtrl.SetBuddy(GetDlgItem(IDC_TIME_EDIT));
	
	//网络运行代码
	nt = new Network();
	nt->ConstructNetwork();

	//nt->ConstructNetworkWithFixedPositon();
	//nt->GatherInformation();
	nt->AdjustPostion();
	
	SetDlgItemText(IDC_TIME_EDIT, _T("0"));
	SetTimer(1, 1, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CShowCaseDlg::ReSize(int nID, int cx, int cy){
	CWnd *pWnd; 
    pWnd = GetDlgItem(nID);     //获取控件句柄
    if(pWnd)   
    {
        CRect rect;   //获取控件变化前大小
        pWnd->GetWindowRect(&rect);
        ScreenToClient(&rect);//将控件大小转换为在对话框中的区域坐标
        //rect.left=(int)(rect.left*((float)cx/(float)m_rect.Width()));//调整控件大小
        rect.right=(int)(rect.right*((float)cx/(float)m_rect.Width()));
       // rect.top=(int)(rect.top*((float)cy/(float)m_rect.Height()));
        rect.bottom = (int)(rect.bottom*((float)cy/(float)m_rect.Height()));
        pWnd->MoveWindow(rect);//设置控件位置
    }
}
void CShowCaseDlg::OnClose(){
	CDialog::OnClose();
	for(size_t i = 0; i < NODE_NUMBER; ++i){
		delete nodes[i];
	}
	delete nt;
}


void CShowCaseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CShowCaseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CShowCaseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CShowCaseDlg::OnEnChangeTimeEdit()
{
	
	if(m_currentTime != 0 && m_currentTime == GetDlgItemInt(IDC_TIME_EDIT)){
		return;//没有更改值
	}
	UpdateData(true);
	CString m_currentTimeString;
	GetDlgItemText(IDC_TIME_EDIT, m_currentTimeString);
	//清空图片上的所有的点和线
	//绘制新的点和线
	//nodes.clear();
	USES_CONVERSION; 
	string m_timeFile = "da/";
	m_timeFile += (W2A(m_currentTimeString));
	m_timeFile += ".txt";
	ifstream  inputFile(m_timeFile, ios_base::in);
	if(inputFile.fail()){
		MessageBox(_T("文件") + m_currentTimeString + _T(".txt打开失败！"), _T("FAIL"), MB_OKCANCEL | MB_ICONQUESTION);
	}else{
		//MessageBox(_T("文件") + m_currentTimeString + _T(".txt打开成功！"), _T("GOOD"), MB_OKCANCEL | MB_ICONQUESTION);
		int id =-1;
		double xAxis = 0.0, yAxis = 0.0;
		int isGateway;
		int workState;
		int relaTime = 0, workTime = 0, totTime = 0;
		double capacity, tranRange;
		string re("");
		while(!inputFile.eof()){
			inputFile>>id
				>>xAxis
				>>yAxis
				>>isGateway
				>>workState
				>>capacity
				>>relaTime
				>>workTime
				>>totTime
				>>tranRange
				>>re;
			(nodes[id])->CopyNode(id, xAxis, yAxis, (NodeState)isGateway, (NodeWorkState)workState, capacity, totTime, relaTime, workTime, tranRange, re);
		}
		inputFile.close();
		//画点
		CRect rectPicture;
		m_pic.GetClientRect(&rectPicture);
		CDC* pDC = m_pic.GetDC();
		CPen newPen;       // 用于创建新画笔   
		CPen *pOldPen;     // 用于存放旧画笔   
		CBrush newBrush;   // 用于创建新画刷   
		CBrush *pOldBrush; // 用于存放旧画刷   
  
		// 创建黑色新画刷   
		newBrush.CreateSolidBrush(RGB(0,0,0));   
		// 选择新画刷，并将旧画刷的指针保存到pOldBrush   
		pOldBrush = pDC->SelectObject(&newBrush);   
		// 以黑色画刷为绘图控件填充黑色，形成黑色背景   
		pDC->Rectangle(rectPicture);   
		// 恢复旧画刷   
		pDC->SelectObject(pOldBrush);   
		// 删除新画刷   
		newBrush.DeleteObject();   
  //
		//// 创建实心画笔，粗度为1，颜色为   
		newPen.CreatePen(PS_SOLID, 1, RGB(255,255,255));   
		//// 选择新画笔，并将旧画笔的指针保存到pOldPen   
		pOldPen = pDC->SelectObject(&newPen);   
  //
		//// 将当前点移动到绘图控件窗口的左下角，以此为波形的起始点   
		//pDC->MoveTo(rectPicture.left, rectPicture.bottom);   
		
		// 计算m_nzValues数组中每个点对应的坐标位置，并依次连接，最终形成曲线   
		for (int i = 0; i < NODE_NUMBER; i++)   
		{   
			if(i == 0){
				pDC->FillSolidRect((int)(nodes[i]->GetXLoc()), (int)(nodes[i]->GetYLoc()), 4 * RADIUS, 4 * RADIUS, RGB(0,0,255));
			}else{
				if(nodes[i]->GetNodeWorkState() == Sleeping){
					pDC->FillSolidRect((int)(nodes[i]->GetXLoc()), (int)(nodes[i]->GetYLoc()), 2 * RADIUS, 2 * RADIUS, RGB(0,191,255));
				}else{
					if(nodes[i]->IsGatewayNodeOrNot() == NonGatewayNode){
						pDC->FillSolidRect((int)(nodes[i]->GetXLoc()), (int)(nodes[i]->GetYLoc()), 2 * RADIUS, 2 * RADIUS, RGB(0,255,0));
					}else{
						pDC->FillSolidRect((int)(nodes[i]->GetXLoc()), (int)(nodes[i]->GetYLoc()), 2 * RADIUS, 2 * RADIUS, RGB(255,0,0));
					}
				}
			}
			for(int j = 0; j < NODE_NUMBER; ++j){
				if(i == j){
					continue;
				}
				if(DIS(nodes[i]->GetXLoc(), nodes[i]->GetYLoc(),nodes[j]->GetXLoc(), nodes[j]->GetYLoc()) < nodes[i]->GetTransDis()){
					pDC->MoveTo(nodes[i]->GetXLoc() + RADIUS, nodes[i]->GetYLoc() + RADIUS); 
					pDC->LineTo(nodes[j]->GetXLoc() + RADIUS, nodes[j]->GetYLoc() + RADIUS);
				}
			}
		}   
  
		//// 恢复旧画笔   
		pDC->SelectObject(pOldPen);   
		//// 删除新画笔   
		newPen.DeleteObject(); 
	}
	
}


void CShowCaseDlg::OnStnClickedPicStatic()
{
	CRect rect;
	GetDlgItem(IDC_PIC_STATIC)->GetWindowRect(&rect);
	//ScreenToClient(&rect);
	//GetDlgItem(IDC_PIC_STATIC)->GetClientRect(&rect2);//获取控件的客户区坐标，里面没有加系统区坐标
	//ClientToScreen(&rect2);//转换为屏幕坐标，客户区坐标相对于系统区的坐标是（12,12），可以通过先获得WindowRect，再转换screen2client
	//
	CPoint point;
	GetCursorPos(&point);
	if(point.x < rect.left || point.y < rect.top){
		return;
	}
	if(!m_flag){
		for (size_t i = 0; i < NODE_NUMBER; i++){
			/*double yy = networkSnapshot[i]->GetXLoc() + (double)rect.left;
			double zz =  networkSnapshot[i]->GetYLoc() + (double)rect.top;
			double xx = DIS((double)point.x, (double)point.y, networkSnapshot[i]->GetXLoc() + (double)rect.left , networkSnapshot[i]->GetYLoc() + (double)rect.top);
			double tt = DIS((double)point.x, (double)point.y, yy, zz);*/
			if(DIS(point.x, point.y, nodes[i]->GetXLoc() + rect.left , nodes[i]->GetYLoc() + rect.top) <= RADIUS){
				m_currentNodeId = nodes[i]->GetId();
				m_currentNodeEnergy = nodes[i]->GetBattery()->GetCapacity();
				m_curerntNodeWorkingTime = nodes[i]->GetWorkingTime();
			
				if(nodes[i]->GetNodeWorkState() == Receiving){
					m_currentWorkType = _T("Receiving");
				}else{
					m_currentWorkType = _T("Sleeping");
				}
			
				if(nodes[i]->IsGatewayNodeOrNot() == GatewayNode){
					m_currentNodeStatus = _T("Gateway");
				}else{
					if(nodes[i]->IsGatewayNodeOrNot() == NonGatewayNode){
						m_currentNodeStatus = _T("NonGate");
					}else{
						m_currentNodeStatus = _T("WhoKnows");
					}
				}
				m_currentNodeRelaxedTime = nodes[i]->GetRelaxingTime();
				m_currentNodeTotalWorkTime = nodes[i]->GetTotalWorkTime();
				USES_CONVERSION; 
				m_allNeighbors = A2W(nodes[i]->GetAllNeighborsText().c_str());
				UpdateData(false);
				return;
			}
		}
	}else{
		m_flag = false;
		if(m_currentNodeId == -1){
			return;
		}
		nodes[m_currentNodeId]->CopyNode((int)m_currentNodeId, point.x - rect.left, point.y - rect.top, NonGatewayNode, Receiving, INITIAL_NORM_ENERGY, 0, 0, 0, COMMUNICATION_RANGE, "");
		stringstream ss;
		string s;
		s.clear();
		ss.str("");
		ss<<0;
		s = "da/";
		s += ss.str();
		s += ".txt";
		ofstream outPutSnapshot(s, ios_base::in | ios_base::trunc);
		if(outPutSnapshot.fail()){
			ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
			assert(!outLogFile.fail());
			outLogFile<<__TIME__<<"Network Run: Error opening this file:snapshot"<<endl;
			outLogFile.close();
			return;
		}
		
		for(int n = 0; n < NODE_NUMBER; ++n){
			string re(",");
			for(size_t i = 0; i < nodes[n]->GetNeighbors().size(); ++i){
				stringstream neis;
				neis<<nodes[n]->GetNeighbors()[i];
				re += neis.str();
				re += ",";
			}
			outPutSnapshot<<nodes[n]->GetId()<<"\t"
				<<(nodes[n]->GetXLoc())<<"\t"
				<<(nodes[n]->GetYLoc())<<"\t"
				<<nodes[n]->IsGatewayNodeOrNot()<<"\t"
				<<nodes[n]->GetNodeWorkState()<<"\t"
				<<nodes[n]->GetBattery()->GetCapacity()<<"\t"
				<<nodes[n]->GetRelaxingTime()<<"\t"
				<<nodes[n]->GetWorkingTime()<<"\t"
				<<nodes[n]->GetTotalWorkTime()<<"\t"
				<<nodes[n]->GetTransDis()<<"\t"
				<<re
				<<endl;
		}
		outPutSnapshot.close();
		SetTimer(1, 1, NULL);
		m_currentNodeId = -1;
	}
}

void CShowCaseDlg::OnSize(UINT nType, int cx, int cy) 
{
    CDialog::OnSize(nType, cx, cy); 
    // TODO: Add your message handler code here
    if(nType!=SIZE_MINIMIZED) //判断是否为最小化
    {
    //ReSize(IDC_OK, cx, cy);
    GetClientRect(&m_rect);
    }
}


void CShowCaseDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	OnEnChangeTimeEdit();
	KillTimer(nIDEvent);
}





void CShowCaseDlg::OnStnDblclickPicStatic()
{
	// TODO: 在此添加控件通知处理程序代码
	//MessageBox(__T("ALALA！"), _T("FAIL"), MB_OKCANCEL | MB_ICONQUESTION);
	if(m_po){
		return;
	}
	m_flag = false;
	OnStnClickedPicStatic();
	m_flag = true;
	
}


void CShowCaseDlg::OnBnClickedRunbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_po = true;
	ofstream initialPosFile("IniPos.txt", ios_base::out | ios_base::trunc);
	if(initialPosFile.fail()){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"ConstructNetwork: Error opening this file:fixPos"<<endl;
		outLogFile.close();
		return;
	}
	for(int i = 0; i < NODE_NUMBER; ++i){
		initialPosFile<<nodes[i]->GetId()<<"\t"<<nodes[i]->GetXLoc()<<"\t"<<nodes[i]->GetYLoc()<<endl;
	}
	initialPosFile.close();
	nt->Run();
	((CButton*)GetDlgItem(IDC_RUNBTN))->EnableWindow(FALSE);
	MessageBox(_T("Hope it works well!!!"), _T("DONE!"), MB_OKCANCEL | MB_ICONQUESTION);
	SetDlgItemText(IDC_TIME_EDIT, _T("1"));
	SetTimer(1, 1, NULL);
}
