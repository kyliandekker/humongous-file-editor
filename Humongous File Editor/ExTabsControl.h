#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace HumongousFileEditor
{
	public ref class ExTabsControl : public TabControl
	{
	public:
		ExTabsControl()
		{
			DrawMode = System::Windows::Forms::TabDrawMode::OwnerDrawFixed;
		}

	protected:
		void OnDrawItem(DrawItemEventArgs^ e) override
		{
			System::Drawing::Rectangle^ r = e->Bounds;
			r = GetTabRect(e->Index);
			r->Offset(2, 2);
			r->Width = 5;
			r->Height = 5;
			Brush^ b = (gcnew SolidBrush(Color::Black));
			Pen^ p = (gcnew Pen(b));
			e->Graphics->DrawLine(p, r->X, r->Y, r->X + r->Width, r->Y + r->Height);
			e->Graphics->DrawLine(p, r->X + r->Width, r->Y, r->X, r->Y + r->Height);

			System::String^ titel = this->TabPages[e->Index]->Text;
			System::Drawing::Font^ f = this->Font;
			PointF pointF = PointF(r->X + 5, r->Y);
			e->Graphics->DrawString(titel, f, b, pointF);
		}

		void OnMouseClick(MouseEventArgs^ e) override
		{
			Point p = e->Location;
			if (e->Button == System::Windows::Forms::MouseButtons::Middle)
			{
				for (int i = 0; i < TabCount; i++)
				{
					System::Drawing::Rectangle^ r = GetTabRect(i);
					if (r->Contains(p))
						CloseTab(i);
				}
				return;
			}
			else if (e->Button == System::Windows::Forms::MouseButtons::Left)
			{
				for (int i = 0; i < TabCount; i++)
				{
					System::Drawing::Rectangle^ r = GetTabRect(i);
					r->Offset(2, 2);
					r->Width = 5;
					r->Height = 5;
					if (r->Contains(p))
						CloseTab(i);
				}
			}
		}

		void CloseTab(int i)
		{
			TabPages->Remove(TabPages[i]);
		}
	};
}
