/***************************************************************************
                          latexoutputfilter.h  -  description
                             -------------------
    begin                : Die Sep 16 2003
    copyright            : (C) 2003 by Jeroen Wijnhout
    email                : wijnhout@science.uva.nl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LATEXOUTPUTFILTER_H
#define LATEXOUTPUTFILTER_H

#include "outputfilter.h"
#include "latexoutputinfo.h"
#include <qvaluestack.h>
#include <qstring.h>

/**An object of this class is used to parse the output messages
generated by a TeX/LaTeX-compiler.

@author Thorsten L�ck
  *@author Jeroen Wijnhout
  */

class KTextEdit;

class LOFStackItem
{
public:
	LOFStackItem(const QString & file = QString::null, bool sure = false) : m_file(file), m_reliable(sure) {}

	const QString & file() const { return m_file; }
	void setFile(const QString & file) { m_file = file; }

	bool reliable() const { return m_reliable; }
	void setReliable(bool sure) { m_reliable = sure; }

private:
	QString	m_file;
	bool		m_reliable;
};

class LatexOutputFilter : public OutputFilter
{
    public:
        LatexOutputFilter(LatexOutputInfoArray* LatexOutputInfoArray);
        ~LatexOutputFilter();

        virtual unsigned int Run(QString logfile);

	enum {Start = 0, FileName, FileNameHeuristic, Error, Warning, BadBox, LineNumber};

    protected:
        /**
        Parses the given line for the start of new files or the end of
        old files.
        */
        void updateFileStack(const QString &strLine, short & dwCookie);
	void updateFileStackHeuristic(const QString &strLine, short & dwCookie);

        /**
        Forwards the currently parsed item to the item list.
        */
        void flushCurrentItem();

        // overridings
    public:
        /** Return number of errors etc. found in log-file. */
        void GetErrorCount(int *errors, int *warnings, int *badboxes);
	void clearErrorCount() { m_nErrors=m_nWarnings=m_nBadBoxes=0 ; }

    protected:
        virtual bool OnPreCreate();
        virtual short parseLine(const QString & strLine, short dwCookie);
        virtual bool OnTerminate();

	bool detectError(const QString & strLine, short &dwCookie);
	bool detectWarning(const QString & strLine, short &dwCookie);
	bool detectBadBox(const QString & strLine, short &dwCookie);
	bool detectLaTeXLineNumber(QString & warning, short & dwCookie);
	bool detectBadBoxLineNumber(QString & strLine, short & dwCookie);

	bool fileExists(const QString & name);

    private:

        // types
    protected:
        /**
        These constants are describing, which item types is currently
        parsed.
        */
        enum tagCookies
        {
            itmNone = 0,
            itmError,
            itmWarning,
            itmBadBox
        };

        // attributes
    public:
        /** number or errors detected */
        int m_nErrors;

        /** number of warning detected */
        int m_nWarnings;

        /** number of bad boxes detected */
        int m_nBadBoxes;

    private:
        /**
        Stack containing the files parsed by the compiler. The top-most
        element is the currently parsed file.
        */
        QValueStack<LOFStackItem> m_stackFile;

        /** The item currently parsed. */
        LatexOutputInfo	m_currentItem;

    public:                                                                     // Public attributes
        /** Pointer to list of Latex output information */
        LatexOutputInfoArray *m_InfoList;

	int m_nParens;
};
#endif
