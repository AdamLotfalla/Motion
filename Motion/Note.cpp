#include "Note.h"
#include <wx/wx.h>
#include "MainFrame.h"
#include <wx/dcbuffer.h>

Note::Note(int WIDTH, int HEIGHT, int ROTATION, int X_POS, int Y_POS, wxString TEXT, wxFont FONT, wxColor FCOLOR, wxColor BCOLOR, wxWindow* PARENT, MainFrame* frame) : wxPanel(PARENT, wxID_ANY, wxPoint(X_POS, Y_POS), wxSize(WIDTH, HEIGHT))
{
    width = WIDTH;
    height = HEIGHT;
    rotation = ROTATION;
    x_pos = X_POS;
    y_pos = Y_POS;
    text = TEXT;
    font = FONT;
    fcolor = FCOLOR;
    bcolor = BCOLOR;
    mHold = false;
    mainFrame = frame;

    if (mainFrame == nullptr) {
        wxLogError("MainFrame pointer is null in Note constructor.");
        return;
    }

    borderWidth = 3;
    mHover = false;

    //this->SetBackgroundColour(bcolor);

    stickynote = new wxPanel(this, wxID_ANY, wxPoint(borderWidth/2,borderWidth/2), wxSize(width - borderWidth, height - borderWidth));
    stickynote->SetBackgroundColour(bcolor);
    textContent = new wxStaticText(stickynote, wxID_ANY, text, wxPoint(5,5), wxSize(width-10, height-10), wxST_NO_AUTORESIZE | GCP_JUSTIFY);
    textContent->SetFont(font);
    textContent->SetForegroundColour(fcolor);

    //textContent->Bind(wxEVT_LEFT_DOWN, &Note::onMouseDrag, this);
    //textContent->Bind(wxEVT_LEFT_DOWN, &Note::makeActive, this);
    textContent->Bind(wxEVT_ENTER_WINDOW, &Note::onMouseEnter, this);
    textContent->Bind(wxEVT_LEAVE_WINDOW, &Note::onMouseLeave, this);
    textContent->Bind(wxEVT_LEFT_DOWN, &Note::OnLeftMouseDown, this);
    textContent->Bind(wxEVT_MOTION, &Note::OnMouseMove, this);
    textContent->Bind(wxEVT_LEFT_UP, &Note::OnLeftMouseUp, this);
    this->Bind(wxEVT_PAINT, &Note::NotePaintBorder, this);

    this->SetDoubleBuffered(true);
    this->GetParent()->SetDoubleBuffered(true);
    stickynote->SetDoubleBuffered(true);
    textContent->SetDoubleBuffered(true);
    Refresh();

    //auto temsizer = new wxBoxSizer(wxVERTICAL);
    //temsizer->Add(textContent, 0, wxEXPAND);
    //sizer = new wxGridSizer(1, 1, 0, 0);
    //sizer->Add(textContent, wxSizerFlags().Border(wxALL, 7).Expand());
    //sizer->SetMinSize(width, height);
    //textContent->SetMinSize(wxSize(width, height));


    //stickynote->SetSizerAndFit(sizer);
}

void Note::UpdateNote(int newWidth, int newHeight, wxString newText, wxFont newFont, wxColor newFcolor, wxColor newBcolor)
{
    this->width = newWidth;
    this->height = newHeight;
    textContent->SetLabel(newText);
    textContent->SetFont(newFont);
    stickynote->SetBackgroundColour(newBcolor);
    textContent->SetForegroundColour(newFcolor);

    fcolor = newFcolor;
    bcolor = newBcolor;
    font = newFont;

    Refresh();
    wxLogStatus("Updated; supposed to be");
}

void Note::onMouseEnter(wxMouseEvent& evt) {
    if (!mainFrame->occupied) {
        textContent->SetCursor(wxCURSOR_OPEN_HAND);
        mHover = true;
        wxLogStatus("Entered");
        evt.Skip();
    }
}

void Note::onMouseLeave(wxMouseEvent& evt) {
    if (!mainFrame->occupied) {
        mHover = false;
        wxLogStatus("Exited");
        evt.Skip();
    }
}

void Note::NotePaintBorder(wxPaintEvent& evt) {
    wxPaintDC dc(this);
    
    // Set a thicker black pen for a more visible outline
    dc.SetPen(wxPen(wxColour("#7f7f7f"), borderWidth));
    
    // Set the background brush to transparent
    //wxBrush transparentBrush;
    //transparentBrush.SetStyle(wxBRUSHSTYLE_TRANSPARENT);
    //dc.SetBrush(wxBrush(bcolor));
    
    wxRect rect = GetClientRect();
    //dc.DrawRectangle(0, 0, width, height);
    dc.DrawLine(wxPoint(0, 0), wxPoint(0, height));
    dc.DrawLine(wxPoint(width, 0), wxPoint(width, height));
    dc.DrawLine(wxPoint(0, 0), wxPoint(width, 0));
    dc.DrawLine(wxPoint(0, height), wxPoint(width, height));
    //dc.DrawLine(0, 0, 2, height);
    //dc.DrawLine(width, 0, width + 2, -height);
    //dc.DrawLine(0, height, width, height);
}

void Note::OnLeftMouseDown(wxMouseEvent& evt) {
    if (!mainFrame->occupied) {
        if (mHover && !mHold) { mHold = true; }
        offset = wxGetMousePosition() - this->GetPosition();
        mainFrame->SetActive(this);

        //textContent->Unbind(wxEVT_LEFT_DOWN, &Note::OnLeftMouseDown, this);
        //textContent->Unbind(wxEVT_ENTER_WINDOW, &Note::onMouseEnter, this);
        textContent->Unbind(wxEVT_LEAVE_WINDOW, &Note::onMouseLeave, this);
        wxLogStatus("Occupation is %i", mainFrame->occupied);
    }
}

void Note::OnMouseMove(wxMouseEvent& evt) {
    if (mHold) {
        mainFrame->occupied = true;
        textContent->SetCursor(wxCURSOR_CLOSED_HAND);
        auto mPos = wxGetMousePosition();
        mainFrame->activeNote->SetPosition(mPos - offset);
    }
}

void Note::OnLeftMouseUp(wxMouseEvent& evt) {
    mHold = false;
    mainFrame->occupied = false;
    wxLogStatus("Occupation is %i", mainFrame->occupied);
    //textContent->Bind(wxEVT_ENTER_WINDOW, &Note::onMouseEnter, this);
    textContent->Bind(wxEVT_LEAVE_WINDOW, &Note::onMouseLeave, this);
    //textContent->Bind(wxEVT_LEFT_DOWN, &Note::OnLeftMouseDown, this);
}