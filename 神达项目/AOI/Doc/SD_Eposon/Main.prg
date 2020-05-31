#define ZLimtH -10 '走门型的最高高度
#define ZLimtL -30 '走门型的最低高度
Global Boolean gRunFlag, g_bVisionMsg
Global Integer gRunMod(30)

Function main
		gRunFlag = True
		g_bVisionMsg = True
		Call Init
		Xqt RobotMoveThread
		Xqt QRobotCtrlThread, NoPause
		Xqt VisionMsg
		Do
			If gRunFlag = False Then
				Exit Do
			EndIf
			Wait 2000
		Loop
Fend

Function Init
	Motor On
    Weight 2
    Power Low
    Speed 5
	Accel 5, 5
	SpeedS 20, 20, 20
	AccelS 50, 50
'	VxCalLoad "CameraCalib.caa"
'	String strFileName$
'	String strTemp$
'	String szTemp1$
'	strTemp$ = Str$(1)
'	szTemp1$ = "Points"
'	strFileName$ = ".pts"
'	strFileName$ = "Points" + Str$(2) + ".pts"
'	SavePoints strFileName$
'	LoadPoints strFileName$
Fend

'Function DisError(nPort As Integer)
'	String strErr$
'	strErr$ = "Err:" + Str$(Err)
'	Print #nPort, m_strSendStr$
'Fend

Function RobotMoveThread
	
	Integer nRecvCnt
	Integer nRumModeI
	Integer nRecvCntI
	Integer nPassCntI
	Real dSingleMoveI
	Real dZHightI
	Boolean bParseRstI
	String strMoveRstI$
	String strRecv$
	
	Do
		OpenNet #201 As Server
		Print "Move Server Open..."
		WaitNet #201
		Print "#201 Cleint Connect Success..."
		Do
			nRecvCnt = ChkNet(201)
			If nRecvCnt > 0 Then
				strRecv$ = ""
				Input #201, strRecv$
				Print "#201 RecvData：", strRecv$
				Call DataParseStr(strRecv$, ByRef nRumModeI, ByRef dSingleMoveI, ByRef dZHightI, ByRef nPassCntI, ByRef bParseRstI)
				If bParseRstI Then
					Call RobotMove(nRumModeI, dSingleMoveI, dZHightI, nPassCntI, ByRef strMoveRstI$)
				Else
					strMoveRstI$ = "ER = RecvCMD Error"
				EndIf
				Print #201, strMoveRstI$
				Print "#201 SendMsg: ", strMoveRstI$
			ElseIf nRecvCnt = -3 Then
				CloseNet #201
				Print "#201 Cleint DisConnect..."
				Exit Do
			EndIf
			
			If gRunFlag = False Then
				CloseNet #201
				Print "#201 Cleint DisConnect..."
				Exit Do
			EndIf
			
		Loop
		
		If gRunFlag = False Then
			Exit Do
		EndIf
	Loop
		
Fend

Function QRobotCtrlThread
	
	Integer nRecvCnt
	Integer nRumModeI
	Integer nRecvCntI
	Integer nPassCntI
	Real dSingleMoveI
	Real dZHightI
	Boolean bParseRstI
	String strMoveRstI$
	String strRecv$
	
	Do
		OpenNet #202 As Server
		Print "Robot Ctrl Server Open..."
		WaitNet #202
		Print "#202 Cleint Connect Success..."
		Do
			nRecvCnt = ChkNet(202)
			If nRecvCnt > 0 Then
				strRecv$ = ""
				Input #202, strRecv$
				Print "#202 RecvData：", strRecv$
				Call DataParseStr(strRecv$, ByRef nRumModeI, ByRef dSingleMoveI, ByRef dZHightI, ByRef nPassCntI, ByRef bParseRstI)
				If bParseRstI Then
					Call RobotCtrl(nRumModeI, dSingleMoveI, dZHightI, nPassCntI, ByRef strMoveRstI$)
				Else
					strMoveRstI$ = "ER = RecvCMD Error"
				EndIf
				Print #202, strMoveRstI$
				Print "#202 SendMsg: ", strMoveRstI$
			ElseIf nRecvCnt = -3 Then
				CloseNet #202
				Print "#202 Cleint DisConnect..."
				Exit Do
			EndIf
			
			If gRunFlag = False Then
				CloseNet #202
				Print "#202 Cleint DisConnect..."
				Exit Do
			EndIf
			
		Loop
		If gRunFlag = False Then
				Exit Do
		EndIf
	Loop
	

Fend
'InterFace explain: strMoveCmd$(RecvData), nMoveMode(RobotMove: MoveMode, RobotCtrl: CtrlMode), dSingleMove(RobotMove: Single Axis Move Dis, RobotCtrl: Use Speed Set - Speed),
'                   dZHight(RobotMove: Z Axis Move Safety Hight Pos, RobotCtrl: Use Accel Set - Acc),  nPassCnt(RobotMove: Move Point Count, Mulit Point - The Last Point is Finish Pint, another Point is Pass Point),
'                   bParseRst(Parse Data Result).
'Point explain	  : CX, CY, CU, CZ, [CV, CW](Use 6 Axis), Hand, Elbow, Wrist, J4Flag, J6Flag(All 11 Pos Info)
Function DataParseStr(strMoveCmd$ As String, ByRef nMoveMode As Integer, ByRef dSingleMove As Real, ByRef dZHight As Real, ByRef nPassCnt As Integer, ByRef bParseRst As Boolean)
	String toks$(0)
	ParseStr strMoveCmd$, toks$(), " "
	Integer nIndex
	bParseRst = True
	For nIndex = 0 To UBound(toks$)
	Next nIndex
	If toks$(0) = "CMD" Then
		nMoveMode = Val(toks$(1))
		dSingleMove = Val(toks$(2))
        dZHight = Val(toks$(3))
		nPassCnt = Val(toks$(4))
'		If dZHight > ZLimtH Then
'			dZHight = ZLimtH
'		ElseIf dZHight < ZLimtL Then
'			dZHight = ZLimtL
'		EndIf
		dZHight = 0.0
		If nIndex = (nPassCnt * 12 + 5) Then
			If nPassCnt > 0 Then
			   For nIndex = 0 To nPassCnt - 1
			   		P(nIndex) = XY(Val(toks$(5 + 12 * nIndex)), Val(toks$(6 + 12 * nIndex)), Val(toks$(7 + 12 * nIndex)), Val(toks$(8 + 12 * nIndex)), Val(toks$(9 + 12 * nIndex)), Val(toks$(10 + 12 * nIndex)))
			   		Hand P(nIndex), Val(toks$(11 + 12 * nIndex))
			   		Elbow P(nIndex), Val(toks$(12 + 12 * nIndex))
			   		Wrist P(nIndex), Val(toks$(13 + 12 * nIndex))
			   		J4Flag P(nIndex), Val(toks$(14 + 12 * nIndex))
			   		J6Flag P(nIndex), Val(toks$(15 + 12 * nIndex))
			   		gRunMod(nIndex) = Val(toks$(16 + 12 * nIndex))
			   Next
'			   SavePoints("Points.pts")
			EndIf
		Else
			bParseRst = False
		EndIf
	Else
		bParseRst = False
	EndIf
Fend

Function RobotMove(nMoveMode As Integer, dSingleMove As Real, dZHight As Real, nPassCnt As Integer, ByRef strRst$ As String)
'	OnErr GoTo errHandler0
	OnErr GoTo DisErrorMove
	Integer nIndex
	Select nMoveMode
		'X Xis Move
		Case 1
			 Go Here +X(dSingleMove)
		'Y Xis Move
		Case 2
			 Go Here +Y(dSingleMove)
		'Z Xis Move
		Case 3
			 Go Here +Z(dSingleMove)
		'U Xis Move
		Case 4
			 Go Here +U(dSingleMove)
		'V Xis Move
		Case 5
			 Go Here +V(dSingleMove)
		'W Xis Move
		Case 6
			 Go Here +W(dSingleMove)
			 
			 
		'Jog 1 Move
		Case 11
			 JTran 1, dSingleMove
		'Jog 2 Move
		Case 12
			 JTran 2, dSingleMove
	 	'Jog 3 Move
		Case 13
			 JTran 3, dSingleMove
		'Jog 4 Move
		Case 14
			 JTran 4, dSingleMove
		'Jog 5 Move
		Case 15
			 JTran 5, dSingleMove
	 	'Jog 6 Move
		Case 16
			 JTran 6, dSingleMove
			 
		'Go to Point0 No Jump 
		Case 20
			Go P0
			
		'Go to Point0 Move
		Case 21
'			If dZHight > ZLimtH Then
'				Go Here :Z(ZLimtH)
'				Go P0 :Z(ZLimtH)
'			ElseIf dZHight < ZLimtL Then
'				Go Here :Z(ZLimtL)
'				Go P0 :Z(ZLimtL)
'			Else
				Go Here :Z(dZHight)
				Go P0 :Z(dZHight)
'			EndIf
			Go P0
		'Pass Move Mode	
		Case 22
			For nIndex = 0 To nPassCnt - 1
				Select gRunMod(nIndex)
					Case 0
							Go Here :Z(dZHight)
							Go P(nIndex) :Z(dZHight)
							Go P(nIndex)
					Case 1
							If nIndex = nPassCnt - 1 Then
'								If dZHight > ZLimtH Then
'									Go Here :Z(ZLimtH)
'									Go P0 :Z(ZLimtH)
'								ElseIf dZHight < ZLimtL Then
'									Go Here :Z(ZLimtL)
'									Go P0 :Z(ZLimtL)
'								Else
									Go Here :Z(dZHight)
									Go P0 :Z(dZHight)
'								EndIf
								Go P0
							Else
								Pass P(nIndex)
							EndIf
							
					Case 2
							Jump P(nIndex) LimZ dZHight
					Case 3
							Move P(nIndex)
				Send
				
			Next
		'	Go P(nIndex)
		'Close Main
		Case 100
			gRunFlag = False
	Send
	
	strRst$ = "OK "
	
	Exit Function
DisErrorMove:
	If Err <> 0 Then
		strRst$ = "ER " + ErrMsg$(Err, 4)
	Else
		strRst$ = "OK "
	EndIf
	
Fend

Function RobotCtrl(ByVal nMoveMode As Integer, ByVal dSpeed As Real, ByVal dAcc As Real, ByVal nPassCnt As Integer, ByRef strRst$ As String)
	OnErr GoTo QRobotCtrl
	Integer nSpeed
	Integer nAcc
	String strTemp$
	nSpeed = dSpeed
	nAcc = dAcc
	Select nMoveMode
		   'Motor On
		   Case 1
		   		Motor On
		   'Motor Off
		   Case 2
		   		Motor Off
		   'Power Low
		   Case 3
		   		Power Low
		   'Power High
		   Case 4
		   		Power High
		   'Chk Power
		   Case 5
		   		If Power = Low Then
		   			strTemp$ = "Low"
		   		Else
		   			strTemp$ = "High"
		   		EndIf
		   'Chk On-Off
		   Case 6
		   		If Motor = On Then
		   			strTemp$ = "On"
		   		Else
		   			strTemp$ = "Off"
		   		EndIf
		   	'Set Speed and Acc
		   	Case 10
		   		 If nSpeed < 10 Then
		   		 	Power Low
		   		 ElseIf nSpeed >= 10 And nSpeed < 101 Then
		   		 	Speed nSpeed
		   		 EndIf
		   		
		   	     If nAcc > 120 Then
		   		 	Accel 20, 20
		   		 Else
		   		 	Accel nAcc, nAcc
		   		 EndIf
		   	'Qry RealPos
		   	Case 15
		   		P99 = RealPos
		   		strTemp$ = Str$(CX(P99)) + " " + Str$(CY(P99)) + " " + Str$(CZ(P99)) + " " + Str$(CU(P99)) + " " + Str$(CV(P99)) + " " + Str$(CW(P99)) + " " + Str$(Hand(P99)) + " " + Str$(Elbow(P99)) + " " + Str$(Wrist(P99)) + " " + Str$(J4Flag(P99)) + " " + Str$(J6Flag(P99))
		   	
		   'Close Main
			Case 100
				gRunFlag = False
		   		
	Send
	
	strRst$ = "OK " + strTemp$
	
	Exit Function
QRobotCtrl:
	If Err <> 0 Then
		strRst$ = "ER " + ErrMsg$(Err, 4)
	Else
		strRst$ = "OK " + strTemp$
	EndIf
Fend


Function VisionMsg
	'SetNet #210, "127.0.0.1", 4010, CRLF, NONE, 0, TCP
	String strRecvMsg$
	String strSendMsg$
	Integer nRecvCnt
	OpenNet #210 As Server 			'打开TCPIP201端口'
	Print "#210端口等待握手。。。"
	WaitNet #210
	Print "#210端口已握手成功。"
	
	Do While g_bVisionMsg
		nRecvCnt = ChkNet(210)
		If nRecvCnt > 0 Then
			strRecvMsg$ = ""
			strSendMsg$ = ""
			Input #210, strRecvMsg$
			Print strRecvMsg$
			ParseMsg(strRecvMsg$, ByRef strSendMsg$)
		ElseIf nRecvCnt = -3 Then
			CloseNet #210
			Print "#210 端口已断开."
			OpenNet #210 As Server 			'打开TCPIP201端口'
			Print "#210端口等待握手。。。"
			WaitNet #210
			Print "#210端口已握手成功。"
		EndIf
		
		If strSendMsg$ <> "" Then 		'如果outdata$不是空字符串'
			  Print #210, strSendMsg$ 	'outdata$里的字符通过201端口发送'		  
			  Print "#210: ", strSendMsg$
			  strSendMsg$ = ""			'outdata$里的字符清空'
		EndIf
	Loop
	
Fend

Function ParseMsg(strMsg$ As String, ByRef strRst$ As String)
	OnErr GoTo DisErrorParseMsg
	String strParseData$(40)
	String strTemp$
	ParseStr strMsg$, strParseData$(), " "
	strTemp$ = ""
	Select Val(strParseData$(1))
		Case 0   							'设置Tool
			Tool Val(strParseData$(2))
			strTemp$ = "OK,"
		Case 1							'设置点位
			P(Val(strParseData$(2))) = XY(Val(strParseData$(3)), Val(strParseData$(4)), Val(strParseData$(5)), Val(strParseData$(6)), Val(strParseData$(7)), Val(strParseData$(8)))
			Hand P(Val(strParseData$(2))), Val(strParseData$(9))
			Elbow P(Val(strParseData$(2))), Val(strParseData$(10))
	   		Wrist P(Val(strParseData$(2))), Val(strParseData$(11))
	   		J4Flag P(Val(strParseData$(2))), Val(strParseData$(12))
	   		J6Flag P(Val(strParseData$(2))), Val(strParseData$(13))
	   		strTemp$ = "OK,"
		Case 2							'计算相机
			CameraCal(Val(strParseData$(2)), Val(strParseData$(3)), ByRef strTemp$)
			strRst$ = strTemp$
		Case 3							'转化坐标
			CalPos(Val(strParseData$(2)), Val(strParseData$(3)), Val(strParseData$(4)), Val(strParseData$(5)), ByRef strTemp$)
			strRst$ = strTemp$
		Case 4
			CreateTool(Val(strParseData$(2)), Val(strParseData$(3)), Val(strParseData$(4)), Val(strParseData$(5)))
			strTemp$ = "OK," + Str$(CX(TLSet(Val(strParseData$(2))))) + "," + Str$(CY(TLSet(Val(strParseData$(2)))))
			
		Case 5
			CreatePallet(Val(strParseData$(2)), Val(strParseData$(3)), Val(strParseData$(4)))
			strTemp$ = "OK,"
		Case 6
	   		P99 = Pallet(Val(strParseData$(2)), Val(strParseData$(3)))
	   		strTemp$ = "OK," + Str$(CX(P99)) + "," + Str$(CY(P99)) + "," + Str$(CZ(P99)) + "," + Str$(CU(P99)) + "," + Str$(CV(P99)) + "," + Str$(CW(P99)) + "," + Str$(Hand(P99)) + "," + Str$(Elbow(P99)) + "," + Str$(Wrist(P99)) + "," + Str$(J4Flag(P99)) + "," + Str$(J6Flag(P99)) + ","
		Case 7
			TLSet Val(strParseData$(2)), XY(Val(strParseData$(3)), Val(strParseData$(4)), Val(strParseData$(5)), Val(strParseData$(6)))
			strTemp$ = "OK,"
		Case 8
			P99 = TLSet(Val(strParseData$(2)))
	   		strTemp$ = "OK," + Str$(CX(P99)) + "," + Str$(CY(P99)) + "," + Str$(CZ(P99)) + "," + Str$(CU(P99)) + "," + Str$(CV(P99)) + "," + Str$(CW(P99)) + "," + Str$(Hand(P99)) + "," + Str$(Elbow(P99)) + "," + Str$(Wrist(P99)) + "," + Str$(J4Flag(P99)) + "," + Str$(J6Flag(P99)) + ","
	Send
	
DisErrorParseMsg:
	If Err <> 0 Then
		strRst$ = "ER," + ErrMsg$(Err, 4)
	Else
		strRst$ = strTemp$
	EndIf
Fend


Function CreateTool(tl_num As Integer, rXVal As Real, rYVal As Real, iCardNo As Integer) 'p_num As Integer,
	Tool 0
	VxCalLoad "CameraCalib.caa"
'	Boolean found
	Real x, y ', u
	Real x0, y0, u0
	Real xTool, yTool, rTool, theta
	Tool 0
'	VRun findTip
'	VGet findTip.tip.RobotXYU, found, x, y, u
	x = CX(VxTrans(iCardNo, XY(rXVal, rYVal, 0, 0)))
	y = CY(VxTrans(iCardNo, XY(rXVal, rYVal, 0, 0)))
'	If found Then
	' Get the TOOL 0 position
	x0 = CX(Here)
	y0 = CY(Here)
	u0 = CU(Here)
	' Calculate initial tool offsets
	' X and Y distance from tip found with vision
	' to center of U axis
	xTool = x - x0
	yTool = y - y0
	' Calculate angle at initial offsets
	theta = Atan2(xTool, yTool)
	' Calculate angle of tool when U is at zero degrees
	theta = theta - DegToRad(u0)
	' Calculate tool radius
	rTool = Sqr(xTool * xTool + yTool * yTool)
	' Calculate final tool offsets
	xTool = Cos(theta) * rTool
	yTool = Sin(theta) * rTool
	' Set the tool
	TLSet tl_num, XY(xTool, yTool, 0, 0) '6轴为-yTool，4轴为yTool
'	EndIf
Fend

Function CameraCal(iCardNo As Integer, iType As Integer, ByRef strRst$ As String)

	Select iType
		Case 0
			VxCalib iCardNo, VISION_CAMORIENT_MOBILEJ4, P(900:908), P(910:918), P919   '生成校准'
		Case 1
			VxCalib iCardNo, VISION_CAMORIENT_FIXEDUP, P(900:908), P(910:918)
		Default
			VxCalib iCardNo, VISION_CAMORIENT_MOBILEJ4, P(900:908), P(910:918), P919   '生成校准'
	Send
    
    If VxCalInfo(iCardNo, 1) = True Then   '如果校准成功'
        Print "移动向下相机校准成功（按键部分），结果如下:"
        Print "          X方向平均偏差[mm]:", VxCalInfo(iCardNo, 2)
        Print "          X方向最大偏差[mm]:", VxCalInfo(iCardNo, 3)
        Print " X方向比例值mm/pix( mm ):", VxCalInfo(iCardNo, 4)
        Print "             X方向傾( deg ):", VxCalInfo(iCardNo, 5)
        Print "          Y方向平均偏差[mm]:", VxCalInfo(iCardNo, 6)
        Print "          Y方向最大偏差[mm]:", VxCalInfo(iCardNo, 7)
        Print " Y方向比例值mm/pix( mm ):", VxCalInfo(iCardNo, 8)
        Print "          Y方向傾( deg ):", VxCalInfo(iCardNo, 9)
        
        strRst$ = "OK," + Str$(VxCalInfo(iCardNo, 2)) + "," + Str$(VxCalInfo(iCardNo, 3)) + "," + Str$(VxCalInfo(iCardNo, 4)) + "," + Str$(VxCalInfo(iCardNo, 5)) + "," + Str$(VxCalInfo(iCardNo, 6)) + "," + Str$(VxCalInfo(iCardNo, 7)) + "," + Str$(VxCalInfo(iCardNo, 8)) + "," + Str$(VxCalInfo(iCardNo, 9))
    Else
        Print "相机校准失败，请重新示教点校准相机！"
        strRst$ = "ER,Cail Fail!"
    EndIf
	
    If VxCalInfo(iCardNo, 1) = True Then
    	VxCalSave "CameraCalib.caa"
    	VxCalLoad "CameraCalib.caa"
    EndIf

Fend



Function CalPos(rXVal As Real, rYVal As Real, rUVal As Real, iCardNo As Integer, ByRef strRst$ As String)
	Tool 0
	VxCalLoad "CameraCalib.caa"
 	P920 = VxTrans(1, XY(rXVal, rYVal, 0, 0))
 	P921 = XY(CX(P920), CY(P920), CZ(Here), CU(Here) + rUVal)
 	strRst$ = "OK," + Str$(CX(P921)) + "," + Str$(CY(P921)) + "," + Str$(CZ(P921)) + "," + Str$(CU(P921)) + "," + Str$(CV(P921)) + "," + Str$(CW(P921)) + "," + Str$(Hand(P921)) + "," + Str$(Elbow(P921)) + "," + Str$(Wrist(P921)) + "," + Str$(J4Flag(P921)) + "," + Str$(J6Flag(P921))
Fend

Function CreatePallet(iPalNo As Integer, iRow As Integer, iCol As Integer)
 	Pallet iPalNo, P1, P2, P3, iRow, iCol
Fend


