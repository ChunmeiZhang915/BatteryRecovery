
// ShowCaseDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ShowCase.h"
#include "ShowCaseDlg.h"
#include "afxdialogex.h"
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CShowCaseDlg �Ի���



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


// CShowCaseDlg ��Ϣ�������

BOOL CShowCaseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//CRect temprect(0,0,1640,1480);
  // SetWindowPos(NULL,0,0,temprect.Width(),temprect.Height(),SWP_NOZORDER|SWP_NOMOVE);
//	SetWindowPos(NULL, 0, 0, 1000, 500, SWP_SHOWWINDOW );
	CaculateCurrent();
	CenterWindow();
	GetClientRect(&m_rect);//�õ���ǰ�Ի���Ĵ�С
	m_timeSpinCtrl.SetRange(0, SIMULATION_TIME);
	m_timeSpinCtrl.SetBuddy(GetDlgItem(IDC_TIME_EDIT));
	
	//�������д���
	nt = new Network();
	nt->ConstructNetwork();

	//nt->ConstructNetworkWithFixedPositon();
	//nt->GatherInformation();
	nt->AdjustPostion();
	
	SetDlgItemText(IDC_TIME_EDIT, _T("0"));
	SetTimer(1, 1, NULL);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}
void CShowCaseDlg::ReSize(int nID, int cx, int cy){
	CWnd *pWnd; 
    pWnd = GetDlgItem(nID);     //��ȡ�ؼ����
    if(pWnd)   
    {
        CRect rect;   //��ȡ�ؼ��仯ǰ��С
        pWnd->GetWindowRect(&rect);
        ScreenToClient(&rect);//���ؼ���Сת��Ϊ�ڶԻ����е���������
        //rect.left=(int)(rect.left*((float)cx/(float)m_rect.Width()));//�����ؼ���С
        rect.right=(int)(rect.right*((float)cx/(float)m_rect.Width()));
       // rect.top=(int)(rect.top*((float)cy/(float)m_rect.Height()));
        rect.bottom = (int)(rect.bottom*((float)cy/(float)m_rect.Height()));
        pWnd->MoveWindow(rect);//���ÿؼ�λ��
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CShowCaseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CShowCaseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CShowCaseDlg::OnEnChangeTimeEdit()
{
	
	if(m_currentTime != 0 && m_currentTime == GetDlgItemInt(IDC_TIME_EDIT)){
		return;//û�и���ֵ
	}
	UpdateData(true);
	CString m_currentTimeString;
	GetDlgItemText(IDC_TIME_EDIT, m_currentTimeString);
	//���ͼƬ�ϵ����еĵ����
	//�����µĵ����
	//nodes.clear();
	USES_CONVERSION; 
	string m_timeFile = "da/";
	m_timeFile += (W2A(m_currentTimeString));
	m_timeFile += ".txt";
	ifstream  inputFile(m_timeFile, ios_base::in);
	if(inputFile.fail()){
		MessageBox(_T("�ļ�") + m_currentTimeString + _T(".txt��ʧ�ܣ�"), _T("FAIL"), MB_OKCANCEL | MB_ICONQUESTION);
	}else{
		//MessageBox(_T("�ļ�") + m_currentTimeString + _T(".txt�򿪳ɹ���"), _T("GOOD"), MB_OKCANCEL | MB_ICONQUESTION);
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
		//����
		CRect rectPicture;
		m_pic.GetClientRect(&rectPicture);
		CDC* pDC = m_pic.GetDC();
		CPen newPen;       // ���ڴ����»���   
		CPen *pOldPen;     // ���ڴ�žɻ���   
		CBrush newBrush;   // ���ڴ����»�ˢ   
		CBrush *pOldBrush; // ���ڴ�žɻ�ˢ   
  
		// ������ɫ�»�ˢ   
		newBrush.CreateSolidBrush(RGB(0,0,0));   
		// ѡ���»�ˢ�������ɻ�ˢ��ָ�뱣�浽pOldBrush   
		pOldBrush = pDC->SelectObject(&newBrush);   
		// �Ժ�ɫ��ˢΪ��ͼ�ؼ�����ɫ���γɺ�ɫ����   
		pDC->Rectangle(rectPicture);   
		// �ָ��ɻ�ˢ   
		pDC->SelectObject(pOldBrush);   
		// ɾ���»�ˢ   
		newBrush.DeleteObject();   
  //
		//// ����ʵ�Ļ��ʣ��ֶ�Ϊ1����ɫΪ   
		newPen.CreatePen(PS_SOLID, 1, RGB(255,255,255));   
		//// ѡ���»��ʣ������ɻ��ʵ�ָ�뱣�浽pOldPen   
		pOldPen = pDC->SelectObject(&newPen);   
  //
		//// ����ǰ���ƶ�����ͼ�ؼ����ڵ����½ǣ��Դ�Ϊ���ε���ʼ��   
		//pDC->MoveTo(rectPicture.left, rectPicture.bottom);   
		
		// ����m_nzValues������ÿ�����Ӧ������λ�ã����������ӣ������γ�����   
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
  
		//// �ָ��ɻ���   
		pDC->SelectObject(pOldPen);   
		//// ɾ���»���   
		newPen.DeleteObject(); 
	}
	
}


void CShowCaseDlg::OnStnClickedPicStatic()
{
	CRect rect;
	GetDlgItem(IDC_PIC_STATIC)->GetWindowRect(&rect);
	//ScreenToClient(&rect);
	//GetDlgItem(IDC_PIC_STATIC)->GetClientRect(&rect2);//��ȡ�ؼ��Ŀͻ������꣬����û�м�ϵͳ������
	//ClientToScreen(&rect2);//ת��Ϊ��Ļ���꣬�ͻ������������ϵͳ���������ǣ�12,12��������ͨ���Ȼ��WindowRect����ת��screen2client
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
    if(nType!=SIZE_MINIMIZED) //�ж��Ƿ�Ϊ��С��
    {
    //ReSize(IDC_OK, cx, cy);
    GetClientRect(&m_rect);
    }
}


void CShowCaseDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	OnEnChangeTimeEdit();
	KillTimer(nIDEvent);
}





void CShowCaseDlg::OnStnDblclickPicStatic()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//MessageBox(__T("ALALA��"), _T("FAIL"), MB_OKCANCEL | MB_ICONQUESTION);
	if(m_po){
		return;
	}
	m_flag = false;
	OnStnClickedPicStatic();
	m_flag = true;
	
}


void CShowCaseDlg::OnBnClickedRunbtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
