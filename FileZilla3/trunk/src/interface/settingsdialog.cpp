#include "FileZilla.h"
#include "settingsdialog.h"
#include "Options.h"
#include "Optionspage.h"
#include "optionspage_passive.h"
#include "optionspage_filetype.h"

enum pagenames
{
	page_none = -1,
	page_filetype = 0,
	page_passive = 1
};

// Helper macro to add pages in the most simplistic way
#define ADD_PAGE(name, classname, parent) \
	wxASSERT(parent < (int)m_pages.size()); \
	page.page = new classname; \
	if (parent == page_none) \
		page.id = treeCtrl->AppendItem(root, name); \
	else \
		page.id = treeCtrl->AppendItem(m_pages[parent].id, name); \
	m_pages.push_back(page);

BEGIN_EVENT_TABLE(CSettingsDialog, wxDialog)
EVT_TREE_SEL_CHANGED(XRCID("ID_TREE"), CSettingsDialog::OnPageChanged)
END_EVENT_TABLE()

CSettingsDialog::CSettingsDialog(COptions* pOptions)
{
	m_pOptions = pOptions;
	m_activePanel = 0;
}

CSettingsDialog::~CSettingsDialog()
{
}

bool CSettingsDialog::Create(wxWindow* parent)
{
	SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
	SetParent(parent);
	if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_SETTINGS")))
		return false;

	if (!LoadPages())
		return false;

	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);

	return true;
}

bool CSettingsDialog::LoadPages()
{
	// Get the tree control.
	wxWindow* wnd = FindWindow(XRCID("ID_TREE"));
	wxTreeCtrl* treeCtrl = wxDynamicCast(wnd, wxTreeCtrl);
	wxASSERT(treeCtrl);
	if (!treeCtrl)
		return false;

	wxTreeItemId root = treeCtrl->AddRoot(_T(""));

	// Create the instances of the page classes and fill the tree.
	t_page page;
	ADD_PAGE(_T("Transfer Mode"), COptionsPagePassive, page_none);
	ADD_PAGE(_T("File Types"), COptionsPageFiletype, page_none);

	// Before we can initialize the pages, get the target panel in the settings
	// dialog.
    wnd = FindWindow(XRCID("ID_PAGEPANEL"));
	wxPanel* parentPanel = wxDynamicCast(wnd, wxPanel);
	wxASSERT(parentPanel);
	if (!parentPanel)
		return false;
	
	// Keep track of maximum page size
	wxSize size;

	for (std::vector<t_page>::iterator iter = m_pages.begin(); iter != m_pages.end(); iter++)
	{
		if (!iter->page->CreatePage(m_pOptions, parentPanel, size))
			return false;
	}

	parentPanel->SetSizeHints(size);

	// Adjust pages sizes according to maximum size
	for (std::vector<t_page>::iterator iter = m_pages.begin(); iter != m_pages.end(); iter++)
	{
		iter->page->GetSizer()->SetMinSize(size);
		iter->page->GetSizer()->Fit(iter->page);
		iter->page->GetSizer()->SetSizeHints(iter->page);
	}

	// Select first page
	treeCtrl->SelectItem(m_pages[0].id);
	if (!m_activePanel)
	{
		m_activePanel = m_pages[0].page;
		m_activePanel->Show();
	}

	return true;
}

void CSettingsDialog::OnPageChanged(wxTreeEvent& event)
{
	if (m_activePanel)
		m_activePanel->Hide();

	wxTreeItemId item = event.GetItem();

	unsigned int size = m_pages.size();
	for (unsigned int i = 0; i < size; i++)
	{
		if (m_pages[i].id == item)
		{
			m_activePanel = m_pages[i].page;
			m_activePanel->Show();
			break;
		}
	}
}
