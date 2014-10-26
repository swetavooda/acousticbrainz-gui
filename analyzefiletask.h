#ifndef FPSUBMIT_ANALYZEFILETASK_H_
#define FPSUBMIT_ANALYZEFILETASK_H_

#include <QRunnable>
#include <QObject>
#include <QStringList>
#include <QProcess>
#include <QTemporaryFile>

struct AnalyzeResult
{
	AnalyzeResult() : error(false)
	{
	}

	QString fileName;
	QString outputFileName;
	int exitCode;
	bool error;
	bool nombid;
	QString errorMessage;
};

class AnalyzeFileTask : public QObject
{
	Q_OBJECT

public:
	AnalyzeFileTask(const QString &path, const QString &file);
	void doanalyze();
	void terminate();
    QString filePath() const { return m_path; }

signals:
	void finished(AnalyzeResult *result);

private slots:
	void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void error(QProcess::ProcessError);

private:
	QString m_path;
	QString m_profile;

	QProcess* extractor;
	QTemporaryFile* tmp;
	AnalyzeResult *result;
};

#endif
