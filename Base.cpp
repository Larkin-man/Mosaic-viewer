//---------------------------------------------------------------------------
#include <stdio.h>
#include <vcl.h>
#pragma hdrstop

#include "Base.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
const int IX = 0;
const int IY = 1;
const int IC = 2;
const int IM = 3;
const int IL = 4;
const int IHP = 5;
const int IZ = 0;
const int NAME = 0;
const int MOZX = 127;
const int MOZY = 133;
const int CENX = 39;
const int CENY = 28;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
	MAPROWS = 108;
	MAPCOLS = 108;
	nMon = 0;
	OpenPictureDialog1->InitialDir = GetCurrentDir();
	bitmap = NULL;
	Map = NULL;
	Moz = NULL;
	bitmap = new Graphics::TBitmap;
	OpenImage("Data\\MapUp.bmp");
	Out->Brush->Color = clInactiveCaptionText;
	Red << gdFixed;
	SavePos.x = 0;
	SavePos.y = 0;
}
//---------------------------------------------------------------

void __fastcall TForm1::OpenBtnClick(TObject *Sender)
{
	if (OpenPictureDialog1->Execute() == ID_OK)
	{
		OpenImage(OpenPictureDialog1->FileName);
	}
}
//---------------------------------------------------------------

void TForm1::OpenImage(const String file)
{
	bitmap->LoadFromFile(file);
	Log->Lines->Add("Изображение:"+IntToStr(bitmap->Width)+"х"+IntToStr(bitmap->Height));
	if (OpenMod->Checked)
	{
		MAPCOLS = nCol->Text.ToInt();
		MAPROWS = nRow->Text.ToInt();
		PixInCell = bitmap->Width / MAPCOLS;
		CellSize->Position = PixInCell;
		//Log->Lines->Add("Размер ячеек рекомендован-1:"+IntToStr(PixInCell));
	}
	else
	{
		PixInCell = CellSize->Position;
		MAPCOLS = bitmap->Width / PixInCell;
		MAPROWS = bitmap->Height / PixInCell;
		if (MAPCOLS <= 0)	MAPCOLS++;
		if (MAPROWS <= 0)	MAPROWS++;
		nCol->Text = MAPCOLS;
		nRow->Text = MAPROWS;
	}
	StretchW = bitmap->Width / MAPCOLS;
	StretchH = bitmap->Height / MAPROWS;
	Out->ColCount = MAPCOLS + Out->FixedCols;
	Out->RowCount = MAPROWS + Out->FixedRows;
	Out->DefaultColWidth = StretchW - 1;
	Out->DefaultRowHeight = StretchH - 1;
	RefreshMapSize();
}
//---------------------------------------------------------------
//---------------------------------------------------------------
void TForm1::RefreshMapSize()
{
	if (bitmap->Width % MAPCOLS == 0)
		nCol->Color = clWindow;
	else
		nCol->Color = clRed;
	if (bitmap->Height % MAPROWS == 0)
		nRow->Color = clWindow;
	else
		nRow->Color = clRed;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	delete bitmap;
	//delete Map;
	if (Moz)
		for (int i = 0; i < MOZX; ++i)
			delete [] Moz[i];
	delete [] Moz;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::eCellSizeChange(TObject *Sender)
{
	Out->DefaultColWidth = CellSize->Position-1;
	Out->DefaultRowHeight = CellSize->Position-1;
//	Stretch = StretchW / CellSize->Position;
}
//---------------------------------------------------------------------------
					 
void __fastcall TForm1::UPMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	if (Button == mbRight)
	{
		Out->MouseToCell(X,Y,X,Y);
		hand.x = X;
		hand.y = Y;  		
		Out->Cursor = crSizeAll;
	} 
	else if (Button == mbLeft)
	{
		Out->MouseToCell(X,Y,X,Y);
		Sel.x = X;
		Sel.y = Y;  
		Out->Cursor = crDrag;//crHandPoint;
		if (Moz)
			Label1->Caption = IntToStr(Moz[X][Y].x)+" "+IntToStr(Moz[X][Y].y);
	}
	else if (Button == mbMiddle)
	{
		Out->MouseToCell(X,Y,X,Y);
		if (Mid == false)
		{
			if ((Mid2.x == X)&&(Mid2.y == Y))
				return;
			Mid1.x = X;
			Mid1.y = Y;
			Label1->Caption = "["+IntToStr(X)+" "+IntToStr(Y)+":";
		}
		else
		{
			if ((Mid1.x == X)&&(Mid1.y == Y))
         	return;				
			Mid2.x = X;
			Mid2.y = Y;
			Label1->Caption = Label1->Caption + IntToStr(X)+" "+IntToStr(Y)+"]";
			RedMid = true;
		}
		Mid = !Mid;
	}
}
//---------------------------------------------------------------------------
		 
void __fastcall TForm1::UPMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
			 int X, int Y)
{
	if (Button == mbRight)
	{
		Out->Cursor = crDefault;
		if ((hand.x == -1)||(hand.y == -1))
			return;      			
		Out->MouseToCell(X,Y,X,Y);
		if ((X == -1)||(Y == -1))
			return; 
		hand.x = Out->LeftCol + hand.x - X;
		hand.y =	Out->TopRow  + hand.y - Y;
		if (hand.x <= 0)
			Out->LeftCol = 0;
		else
			Out->LeftCol = hand.x;
		if (hand.y <= 0)
			Out->TopRow = 0;
		else
			Out->TopRow = hand.y;
		//Log->Lines->Add(IntToStr(X)+"XY"+IntToStr(Y));
		//PaintMapClick(Sender);
	}
	else if (Button == mbLeft) //Отпутсил левую
	{
		Out->Cursor = crDefault;
		if (Moz == NULL)
			return;
		Out->MouseToCell(X,Y,X,Y);
		if ((Sel.x == X)&&(Sel.y == Y))
      	return;
		if ((X < 0)||(Y < 0))
			return;
		//MCELL *p1, *p2;		p1 = &Moz[X][Y];		p2 = &Moz[Sel.x][Sel.y];
		MCELL	temp = Moz[X][Y];
		Moz[X][Y] = Moz[Sel.x][Sel.y];
		Moz[Sel.x][Sel.y] = temp;
		TRect Rect;
		Rect = Out->CellRect(X, Y);
		if ((Moz[X][Y].x == -1)||(Moz[X][Y].y == -1))
			Out->Canvas->FillRect(Rect);
		else
			OutDrawCell(Sender, X, Y, Rect, Red);
		Rect = Out->CellRect(Sel.x, Sel.y);
		if ((Moz[Sel.x][Sel.y].x == -1)||(Moz[Sel.x][Sel.y].y == -1))
			Out->Canvas->FillRect(Rect);
		else               
			OutDrawCell(Sender, Sel.x, Sel.y, Rect, Red);
		//Label1->Caption = Label1->Caption +"->"+IntToStr(X)+" "+IntToStr(Y);
	}      			  
}
//---------------------------------------------------------------------------

void __fastcall TForm1::OutSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect)
{
	CanSelect = CanS->Checked;
	//if ((Sel.x == ACol)&&(Sel.y == ARow))
	//	return;	//Заглушка повотрного выбора ИНАЧЕ ДВАЖДЫ ВСЕГДА пи зрапрете
	//return;
	if (Moz)
		ToLog(IntToStr(ACol)+"xy"+IntToStr(ARow)+":"+IntToStr(Moz[ACol][ARow].x-CENX)+" "+IntToStr(CENY - Moz[ACol][ARow].y));
	else
	{
		ToLog(IntToStr(ACol)+"xy"+IntToStr(ARow));
		return;
	}
	if (RedMid)//(Label1->Caption[Label1->Caption.Length()] == ']')
	{
		MCELL temp;
		int tox = ACol;
		int toy = ARow;
		if (Mid1.x > Mid2.x)
		{
			temp.x = Mid1.x;
			Mid1.x = Mid2.x;
			Mid2.x = temp.x;
			tox += Mid1.x - Mid2.x;						
		}   
		if (Mid1.y > Mid2.y)
		{
			temp.y = Mid1.y;
			Mid1.y = Mid2.y;
			Mid2.y = temp.y;
			toy += Mid1.y - Mid2.y;						
		}  
		//ToLog(IntToStr(Mid1.x)+" "+IntToStr(Mid1.y));
		//ToLog(IntToStr(Mid2.x)+" "+IntToStr(Mid2.y));
		//ToLog(IntToStr(tox)+" "+IntToStr(toy));
		tox -= Mid1.x;
		toy -= Mid1.y;
		for (int x = Mid1.x; x <= Mid2.x; ++x)
			for (int y = Mid1.y; y <= Mid2.y; ++y)
			{
				temp = Moz[x][y];
				Moz[x][y] = Moz[x+tox][y+toy];
				Moz[x+tox][y+toy] = temp; 	
			}	 
		Out->Repaint();
		Label1->Caption = ".";//Label1->Caption +"->"+IntToStr(ACol)+" "+IntToStr(ARow);
      RedMid = false;
		//Label1->Caption = IntToStr((int)ACol)+" "+IntToStr((int)ARow);
	}	       	
//	if ((Moz[ACol][ARow].x == -1)||(Moz[ACol][ARow].y == -1))
//		Out->Canvas->FillRect(Out->CellRect(ACol,ARow));
//	else
//		PaintRealInCell(Moz[ACol][ARow].x, Moz[ACol][ARow].y, Sel.x,Sel.y);
//	//Log->Lines->Add(IntToStr((int)re.Left)+"xSELy"+IntToStr((int)re.Top));
	//Log->Lines->Add(IntToStr((int)ACol)+"xSELy"+IntToStr((int)ARow)) ;
	//Log->Lines->Add(IntToStr((int)Out->CellRect(ACol,ARow).Left)+"xRECy"+IntToStr((int)Out->CellRect(ACol,ARow).Top));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::nColChange(TObject *Sender)
{
	MAPCOLS = nCol->Text.ToInt();
	Out->ColCount = MAPCOLS + Out->FixedCols;
	nRow->Hint = bitmap->Height * MAPCOLS / bitmap->Width;
	StretchW = bitmap->Width / MAPCOLS;
	eCellSize->Hint = StretchW;
	RefreshMapSize();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::nRowChange(TObject *Sender)
{
	MAPROWS = nRow->Text.ToInt();
	Out->RowCount = MAPROWS + Out->FixedRows;
	nCol->Hint = bitmap->Width * MAPROWS / bitmap->Height ;
	StretchH = bitmap->Height / MAPROWS;
	eCellSize->Hint = StretchH;
	RefreshMapSize();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::PaintLinesClick(TObject *Sender)
{
	bitmap->Canvas->Pen->Color = clBlack;
	Log->Lines->Add(PixInCell);
	int i = PixInCell;
	if (N1->Checked)
		i = rrx->Text.ToIntDef(0);
	for (; i < bitmap->Width; i+=PixInCell)
	{
		bitmap->Canvas->MoveTo(i,1);
		bitmap->Canvas->LineTo(i,bitmap->Height);
	}
	i = PixInCell;
	if (N1->Checked)
		i = rry->Text.ToIntDef(0);
	for (; i < bitmap->Height; i+=PixInCell)
	{
		bitmap->Canvas->MoveTo(1,i);
		bitmap->Canvas->LineTo(bitmap->Width,i);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::PaintMapClick(TObject *Sender)
{
	UpDest = Out->Canvas->ClipRect; //Вся область грида
		//Log->Lines->Add(UpDest.Width());
	Source = UpDest;
	//UpDest.left += Out->ColWidths[0]+1;  //для фикседровс
	//UpDest.top += Out->RowHeights[0]+1;
				//Log->Lines->Add(Out->TopRow);
				//Log->Lines->Add(Out->LeftCol);
	Source.Left = (Out->LeftCol)*StretchW;
	Source.Top = (Out->TopRow)*StretchH;
	Source.right = Source.left + UpDest.Width() * StretchW / CellSize->Position;
	Source.bottom = Source.top + UpDest.Height()* StretchH / CellSize->Position;
				//Log->Lines->Add(Source.Width());
				//Log->Lines->Add(Source.Height());
	Out->Canvas->CopyRect(Out->Canvas->ClipRect, //Куда
		bitmap->Canvas, //Откуда
		Source);  //Откуда
}
//---------------------------------------------------------------------------

void TForm1::PaintRealInCell(int rx, int ry, int tox, int toy)
{
	Source.Left = (rx)*StretchW;
	Source.Top = (ry)*StretchH;
	Source.right = Source.left + StretchW-1;
	Source.bottom = Source.top + StretchH-1;
	Log->Lines->Add(StretchW);
	Log->Lines->Add(Out->CellRect(tox,toy).Width());
	Out->Canvas->CopyRect(Out->CellRect(tox,toy), //Куда
		bitmap->Canvas, //Откуда
		Source);  //Откуда
}
//------------------------------------------------------------------------
void TForm1::PaintRealInRect(int rx, int ry, TRect to)
{
}
//------------------------------------------------------------------------

void TForm1::ToLog(const String text)
{
	Log->Lines->Add(text);
}
//---------------------------------------------------------------------------
void TForm1::CreateMozaic()
{
	Moz = new MCELL*[MOZX];
	for (int i = 0; i < MOZX; ++i)
		Moz[i] = new MCELL[MOZY];
  	for (int x = 0; x < MOZX; ++x)
		for (int y = 0; y < MOZY; ++y)
		{
			Moz[x][y].x = -1;
         Moz[x][y].y = -1;
		}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SaveClick(TObject *Sender)
{
	if (Moz == NULL)
		return;
	FILE *fp = fopen ("Vafendal.txt", "wt"); //
	if (!fp)
		return;
	fprintf(fp, "%d ", MOZX);
	fprintf(fp, "%d\n", MOZY);
	for (int x = 0; x < MOZX; ++x)
		for (int y = 0; y < MOZY; ++y)
		{
			 fprintf(fp, "%d ", x);
			 fprintf(fp, "%d ", y);
			 fprintf(fp, "%d ", Moz[x][y].x);
			 fprintf(fp, "%d\n", Moz[x][y].y);
		}
	fclose(fp);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LoadClick(TObject *Sender)
{
	FILE *fp = fopen ("Vafendal.txt", "rt");
	if (!fp)
		return;
   if (Moz == NULL)
		CreateMozaic();
	int mx, my, x, y;
	fscanf(fp, "%d ", &mx);
	fscanf(fp, "%d\n", &my);
	for (int i = 0; i < mx; ++i)
		for (int j = 0; j < my; ++j)
		{
			fscanf(fp, "%d ", &x);
			fscanf(fp, "%d ", &y);
			fscanf(fp, "%d ", &Moz[x][y].x);
			fscanf(fp, "%d\n", &Moz[x][y].y);
		}
	fclose(fp);
	nCol->Enabled = false;
	nRow->Enabled = false;
	Out->ColCount = MOZX + Out->FixedCols;
	Out->RowCount = MOZY + Out->FixedRows;
	rrx->Text = 22;
	rry->Text = 0;
	Save->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::OutDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
			 TGridDrawState State)
{
	//if (ACol == 1)
	//	ToLog(IntToStr((int)ACol)+"xy"+IntToStr((int)ARow)+" "+IntToStr(State.ToInt())) ;
	if (Moz)
	{
		if ((Moz[ACol][ARow].x == -1)||(Moz[ACol][ARow].y == -1))
      	return;
		Source.Left = Moz[ACol][ARow].x*StretchW;
		Source.Top = Moz[ACol][ARow].y*StretchH;
		Source.right = Source.left + StretchW;
		Source.bottom = Source.top + StretchH;
		if (Grid->Checked)
		{
			Source.right -= 1;
			Source.bottom -= 1;
		}
		else
		{
			Rect.right += 1;
			Rect.bottom += 1;
      }
		Out->Canvas->CopyRect(Rect, //Куда
			bitmap->Canvas, //Откуда
			Source);  //Откуда
	}
	else
	{
		Source.Left = ACol*StretchW;
		Source.Top = ARow*StretchH;
		Source.right = Source.left + StretchW-1;
		Source.bottom = Source.top + StretchH-1;
		Out->Canvas->CopyRect(Rect, //Куда
			bitmap->Canvas, //Откуда
			Source);  //Откуда
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::OutKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if (Key == VK_SPACE)
	{
		UpDest = Out->Canvas->ClipRect; //Вся область грида
		Source = UpDest;
		int cx = rrx->Text.ToInt();
		int cy = rry->Text.ToInt();
		//UpDest.left += Out->ColWidths[0]+1;  //для фикседровс
		//UpDest.top += Out->RowHeights[0]+1;
					//Log->Lines->Add(Out->TopRow);
					//Log->Lines->Add(Out->LeftCol);
		Source.Left = (Out->LeftCol+(cx-Out->LeftCol)/2.0) * StretchW;
		Source.Top = (Out->TopRow+(cy-Out->TopRow)/2.0) * StretchH;
		Source.right = Source.left + UpDest.Width()/2.0 * StretchW / CellSize->Position;
		Source.bottom = Source.top + UpDest.Height()/2.0 * StretchH / CellSize->Position;
					//Log->Lines->Add(Source.Width());
					//Log->Lines->Add(Source.Height());
		Out->Canvas->CopyRect(Out->Canvas->ClipRect, //Куда
			bitmap->Canvas, //Откуда
			Source);  //Откуда
   }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::OutKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if (Key == VK_SPACE)
		Out->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Label1Click(TObject *Sender)
{
	Mid = false;
	RedMid = false;
	Label1->Caption = "";
}
//---------------------------------------------------------------------------


void __fastcall TForm1::SetPosClick(TObject *Sender)
{
	SavePos.x = Out->LeftCol;
	SavePos.y = Out->TopRow;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::GetPosClick(TObject *Sender)
{
	Out->LeftCol = SavePos.x;
	Out->TopRow = SavePos.y;
}
//---------------------------------------------------------------------------


