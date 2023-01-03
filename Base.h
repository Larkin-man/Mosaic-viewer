//---------------------------------------------------------------------------

#ifndef BaseH
#define BaseH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <ToolWin.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ExtDlgs.hpp>
#include <Menus.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TFlowPanel *FlowPanel1;
	TButton *OpenBtn;
	TDrawGrid *Out;
	TOpenPictureDialog *OpenPictureDialog1;
	TCheckBox *OpenMod;
	TPopupMenu *PopupMenu1;
	TMenuItem *N1;
	TButton *PaintMap;
	TMemo *Log;
	TLabeledEdit *nCol;
	TLabeledEdit *nRow;
	TLabeledEdit *eCellSize;
	TUpDown *CellSize;
	TCheckBox *CanS;
	TButton *PaintLines;
	TLabeledEdit *rrx;
	TLabeledEdit *rry;
	TButton *Save;
	TButton *Load;
	TLabel *Label1;
	TCheckBox *Grid;
	TButton *SetPos;
	TButton *GetPos;
	TButton *OpenMap;
   void __fastcall OpenBtnClick(TObject *Sender);
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall PaintMapClick(TObject *Sender);
	void __fastcall eCellSizeChange(TObject *Sender);
	void __fastcall UPMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall UPMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall nColChange(TObject *Sender);
	void __fastcall nRowChange(TObject *Sender);
	void __fastcall OutSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall PaintLinesClick(TObject *Sender);
	void __fastcall SaveClick(TObject *Sender);
	void __fastcall LoadClick(TObject *Sender);
	void __fastcall OutDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect, TGridDrawState State);
	void __fastcall OutKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall OutKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Label1Click(TObject *Sender);
	void __fastcall SetPosClick(TObject *Sender);
	void __fastcall GetPosClick(TObject *Sender);
	void __fastcall OpenMapClick(TObject *Sender);


private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
	Graphics::TBitmap *bitmap, *Map;
	TRect UpDest, Source;
	int StretchW, StretchH; //bitmap->Width / MAPCOLS;=64
				//bitmap->Width / MAPCOLS;=64
	int PixInCell;
	int MAPROWS;
	int MAPCOLS;
	int nMon;
	void OpenImage(const String file);
	void RefreshMapSize();
	void PaintRealInCell(int rx, int ry, int tox, int toy);
	void PaintRealInRect(int rx, int ry, TRect to);
	void ToLog(const String text);
	struct MCELL
	{
		int x;
		int y;
	} **Moz;
	MCELL hand, Sel;
	void CreateMozaic();
	bool Mid, RedMid;
	TGridDrawState Red;
	MCELL Mid1, Mid2;
   MCELL SavePos;
};
// перетаскивание
//	int a = rrx->Text.ToInt();
//	int b = rry->Text.ToInt();
//	for (int i = Sel.x; i < 127; ++i)
//	{
//		MCELL	temp = Moz[i][a];
//		Moz[i][a] = Moz[i][b];
//		Moz[i][b] = temp;
//	}
//	Out->Repaint();
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
