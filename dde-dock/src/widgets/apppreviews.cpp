#include "apppreviews.h"

AppPreviewFrame::AppPreviewFrame(const QString &title, int xid, QWidget *parent) :
    QWidget(parent),xidValue(xid)
{
    addPreview(xid);
    setTitle(title);
    addCloseButton();
}

AppPreviewFrame::~AppPreviewFrame()
{

}

void AppPreviewFrame::addPreview(int xid)
{
    m_preview = new WindowPreview(xid, this);
    m_preview->resize(Dock::APP_PREVIEW_WIDTH,Dock::APP_PREVIEW_HEIGHT);

    resize(m_preview->width() + BUTTON_SIZE, m_preview->height() + BUTTON_SIZE);
    m_preview->move(BUTTON_SIZE / 2, BUTTON_SIZE / 2);
}

void AppPreviewFrame::setTitle(const QString &t)
{
    QLabel *titleLabel = new QLabel(this);
    titleLabel->setObjectName("AppPreviewFrameTitle");
    QFontMetrics fm(titleLabel->font());
    titleLabel->setText(fm.elidedText(t,Qt::ElideRight,width()));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->resize(width() - BUTTON_SIZE - PREVIEW_BORDER_WIDTH * 2, TITLE_HEIGHT);
    titleLabel->move(BUTTON_SIZE / 2 + PREVIEW_BORDER_WIDTH, height() - titleLabel->height() - BUTTON_SIZE / 2 - PREVIEW_BORDER_WIDTH);
}

void AppPreviewFrame::mousePressEvent(QMouseEvent *)
{
    emit activate(xidValue);
}

void AppPreviewFrame::enterEvent(QEvent *)
{
    m_preview->setIsHover(true);

    showCloseButton();
}

void AppPreviewFrame::leaveEvent(QEvent *)
{
    m_preview->setIsHover(false);

    hideCloseButton();
}

void AppPreviewFrame::addCloseButton()
{
    m_cb = new QPushButton(this);
    m_cb->setObjectName("PreviewCloseButton");
    m_cb->resize(BUTTON_SIZE, BUTTON_SIZE);
    m_cb->move(width() - m_cb->width(), 0);
    m_cb->hide();

    connect(m_cb,&QPushButton::clicked,[=]{close(this->xidValue);});
}

void AppPreviewFrame::showCloseButton()
{
    m_cb->show();
}

void AppPreviewFrame::hideCloseButton()
{
    m_cb->hide();
}

AppPreviews::AppPreviews(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
    resize(Dock::APP_PREVIEW_WIDTH,Dock::APP_PREVIEW_HEIGHT);
}

void AppPreviews::addItem(const QString &title, int xid)
{
    if (m_xidList.indexOf(xid) != -1)
        return;
    m_mainLayout->setMargin(Dock::APP_PREVIEW_MARGIN);
    m_mainLayout->setSpacing(0);
    m_xidList.append(xid);

    AppPreviewFrame *f = new AppPreviewFrame(title, xid);
    connect(f,&AppPreviewFrame::close,this,&AppPreviews::removePreview);
    connect(f,&AppPreviewFrame::activate,this,&AppPreviews::activatePreview);

    m_mainLayout->addWidget(f);

    int contentWidth = m_mainLayout->count() * (f->width() + Dock::APP_PREVIEW_MARGIN) + Dock::APP_PREVIEW_MARGIN;
    resize(contentWidth,f->height() + Dock::APP_PREVIEW_MARGIN*2);
}

void AppPreviews::leaveEvent(QEvent *)
{
    if (m_isClosing)
        m_isClosing = false;
}

void AppPreviews::removePreview(int xid)
{
    m_isClosing = true;
    m_mainLayout->removeWidget(qobject_cast<AppPreviewFrame *>(sender()));
    sender()->deleteLater();
    m_clientManager->CloseWindow(xid);
    if (m_mainLayout->count() <= 0)
    {
        emit requestHide();
        return;
    }

    int contentWidth = m_mainLayout->count()
            * (Dock::APP_PREVIEW_WIDTH + Dock::APP_PREVIEW_MARGIN + Dock::APP_PREVIEW_CLOSEBUTTON_SIZE)
            + Dock::APP_PREVIEW_MARGIN + Dock::APP_PREVIEW_CLOSEBUTTON_SIZE;
    resize(contentWidth,Dock::APP_PREVIEW_HEIGHT + Dock::APP_PREVIEW_CLOSEBUTTON_SIZE + Dock::APP_PREVIEW_MARGIN*2);
    emit sizeChanged();
}

void AppPreviews::activatePreview(int xid)
{
    m_clientManager->ActiveWindow(xid);
}

void AppPreviews::clearUpPreview()
{
    QLayoutItem *child;
    while ((child = m_mainLayout->takeAt(0)) != 0){
        child->widget()->deleteLater();
        delete child;
    }

    m_xidList.clear();
}

AppPreviews::~AppPreviews()
{

}




