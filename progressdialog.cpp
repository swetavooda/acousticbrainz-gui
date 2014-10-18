#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QTreeView>
#include <QPushButton>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include "progressdialog.h"
#include "constants.h"

ProgressDialog::ProgressDialog(QWidget *parent, Extractor *extractor)
	: QDialog(parent), m_extractor(extractor)
{
	setupUi();
    connect(extractor, SIGNAL(fileListLoadingStarted()), SLOT(onFileListLoadingStarted()));
    connect(extractor, SIGNAL(extractionStarted(int)), SLOT(onExtractionStarted(int)));
    connect(extractor, SIGNAL(currentPathChanged(const QString &)), SLOT(onCurrentPathChanged(const QString &)));
    connect(extractor, SIGNAL(finished()), SLOT(onFinished()));
    //connect(fingerprinter, SIGNAL(networkError(const QString &)), SLOT(onNetworkError(const QString &)));
    //connect(fingerprinter, SIGNAL(authenticationError()), SLOT(onAuthenticationError()));
    connect(extractor, SIGNAL(noFilesError()), SLOT(onNoFilesError()));
}

ProgressDialog::~ProgressDialog()
{
}

void ProgressDialog::setupUi()
{
	m_mainStatusLabel = new QLabel(tr("Starting..."));
	m_currentPathLabel = new QLabel();

	m_closeButton = new QPushButton(tr("&Close"));
	connect(m_closeButton, SIGNAL(clicked()), SLOT(close()));

	m_stopButton = new QPushButton(tr("&Stop"));
	connect(m_stopButton, SIGNAL(clicked()), SLOT(stop()));

	m_pauseButton = new QPushButton(tr("&Pause"));
	m_pauseButton->setCheckable(true);
	connect(m_pauseButton, SIGNAL(clicked(bool)), SLOT(togglePause(bool)));

	QDialogButtonBox *buttonBox = new QDialogButtonBox();
	buttonBox->addButton(m_pauseButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_stopButton, QDialogButtonBox::RejectRole);
	buttonBox->addButton(m_closeButton, QDialogButtonBox::RejectRole);
	m_closeButton->setVisible(false);

	m_progressBar = new QProgressBar();
	m_progressBar->setMinimum(0);
	m_progressBar->setMaximum(0);
	m_progressBar->setFormat(tr("%v of %m"));
	m_progressBar->setTextVisible(false);
    connect(m_extractor, SIGNAL(progress(int)), m_progressBar, SLOT(setValue(int)));

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(m_mainStatusLabel);
	mainLayout->addWidget(m_progressBar);
	mainLayout->addWidget(m_currentPathLabel);
	mainLayout->addStretch();
	mainLayout->addWidget(buttonBox);

	setLayout(mainLayout);
	setWindowTitle(tr("Audio Feature Extractor"));
	resize(QSize(450, 200));
}

void ProgressDialog::onFileListLoadingStarted()
{
	m_progressBar->setTextVisible(false);
	m_progressBar->setMaximum(0);
	m_progressBar->setValue(0);
	m_mainStatusLabel->setText(tr("Collecting files..."));
}

void ProgressDialog::onExtractionStarted(int count)
{
	m_progressBar->setTextVisible(true);
	m_progressBar->setMaximum(count);
	m_progressBar->setValue(0);
	m_mainStatusLabel->setText(tr("Extracting..."));
}

void ProgressDialog::onFinished()
{
	m_mainStatusLabel->setText(tr("Submitted %n feature file(s), thank you!", "", m_extractor->submittedExtractions()));
	m_closeButton->setVisible(true);
	m_pauseButton->setVisible(false);
	m_stopButton->setVisible(false);
}

void ProgressDialog::onCurrentPathChanged(const QString &path)
{
    QString elidedPath =
        m_currentPathLabel->fontMetrics().elidedText(
            path, Qt::ElideMiddle, m_currentPathLabel->width());
    m_currentPathLabel->setText(elidedPath);
}

void ProgressDialog::setProgress(int value)
{
	m_progressBar->setValue(value);
}

void ProgressDialog::stop()
{
	m_extractor->cancel();
	m_pauseButton->setEnabled(false);
	m_stopButton->setEnabled(false);
}

/*
void ProgressDialog::onNetworkError(const QString &message)
{
	stop();
	QMessageBox::critical(this, tr("Network Error"), message);
}

void ProgressDialog::onAuthenticationError()
{
	stop();
	QMessageBox::critical(this, tr("Error"),
		tr("Invalid API key. Please check if the API key "
		"you entered matches your key on the "
		"<a href=\"%1\">Acoustid website</a>.").arg(API_KEY_URL));
}
*/

void ProgressDialog::onNoFilesError()
{
	QMessageBox::critical(this, tr("Error"),
		tr("There are no audio files in the selected folder(s)."));
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
	if (!m_extractor->isFinished()) {
		stop();
		event->ignore();
	}
	else {
		event->accept();
	}
}

void ProgressDialog::togglePause(bool checked)
{
	if (checked) {
		m_extractor->pause();
	}
	else {
		m_extractor->resume();
	}
}
