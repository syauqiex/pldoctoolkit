#include "common.h"

VoidT closeProject()
{
	F_ObjHandleT openedDocId, nextDocId, openedBookId, nextBookId;
	StringT fileName, pathName;

	/* Get Id and path of the active document - any from the project */
	openedDocId = F_ApiGetId(FV_SessionId, FV_SessionId, FP_ActiveDoc);
	/* if opened book get id of active book*/
	if (openedDocId == 0)
		openedDocId = F_ApiGetId(FV_SessionId, FV_SessionId, FP_ActiveBook);
	/* exit if no documents are opened*/
	if (openedDocId == 0)
		return;
	fileName = F_ApiGetString(FV_SessionId, openedDocId, FP_Name);
	/* Remember path of the project directory - we will close all files from there */
	pathName = F_StrCopyString(fileName);
	pathFilename(pathName);
	/* from first opened we look through all opened documents */
	openedDocId = F_ApiGetId(FV_SessionId, FV_SessionId, FP_FirstOpenDoc);	
    while (openedDocId != 0)
    {
		nextDocId = F_ApiGetId(FV_SessionId, openedDocId, FP_NextOpenDocInSession);
		fileName = F_ApiGetString(FV_SessionId, openedDocId, FP_Name);
		pathFilename(fileName);
		/* If current file from the project's workspace, save and close it */
		if (F_StrIEqual(pathName, fileName))
		{
			/* Save file and close it */
				fileName = F_ApiGetString(FV_SessionId, openedDocId, FP_Name);
				F_ApiSimpleSave(openedDocId, fileName, False);
				F_ApiClose(openedDocId, FF_CLOSE_MODIFIED);
		}
		openedDocId = nextDocId;
	}	
	/* All the same with books*/
	openedBookId = F_ApiGetId(FV_SessionId, FV_SessionId, FP_FirstOpenBook);	
    while (openedBookId != 0)
    {
		nextBookId = F_ApiGetId(FV_SessionId, openedBookId, FP_NextOpenBookInSession);
		fileName = F_ApiGetString(FV_SessionId, openedBookId, FP_Name);
		pathFilename(fileName);
		if (F_StrIEqual(pathName, fileName))
		{
			/* Save file and close it */
				fileName = F_ApiGetString(FV_SessionId, openedBookId, FP_Name);
				F_ApiSimpleSave(openedBookId, fileName, False);
				F_ApiClose(openedBookId, FF_CLOSE_MODIFIED);
		}
		openedBookId = nextBookId;
	}
}

BoolT cleanDirectory(FilePathT *dirPath)
{
	FilePathT *file;
	StringT path;
	DirHandleT handle;
	IntT statusp;

	handle = F_FilePathOpenDir(dirPath,&statusp);
	if (!handle)
	{
		F_Printf(NULL,"Invalid directory path: %s\n",F_FilePathToPathName(dirPath,FDosPath));
		writeToChannel("Error. Invalid directory path");
		return 0;
	}
	while (file = F_FilePathGetNext(handle,&statusp))
	{
		path = F_FilePathToPathName(file,FDosPath);
		path = fileFileName(path);
		if ((!validateFilename(path,FM) || F_StrISuffix(path,(StringT)".backup.fm")
				|| F_StrISuffix(path,(StringT)".recover.fm"))//documents, which are not backup or recover
			&&(!validateFilename(path,DRL) || F_StrSuffix(path,(StringT)".backup.drl"))//drl documents, which are not recover
			&&(!F_StrIEqual(path,defaultBookName)))//main book
		{
			F_DeleteFile(file);
		}
	}
	F_FilePathCloseDir(handle);
	F_ApiDeallocateString(&path);
	F_FilePathFree(file);
	F_Free(&handle);
	F_Free(&statusp);

	return 1;
}
BoolT validateFilename(StringT str, IntT type)
{
	StringT name;
	switch (type)
	{
	case FM:
		return F_StrISuffix(str,(StringT)".fm")
			&& !F_StrISuffix(str,(StringT)".backup.fm")
			&& !F_StrISuffix(str,(StringT)".recover.fm");
	case FMBOOK:
		return F_StrISuffix(str,(StringT)".book")
			&& !F_StrISuffix(str,(StringT)".backup.book")
			&& !F_StrISuffix(str,(StringT)".recover.book");
	case GENBOOK:
		name = str;
		name = fileFileName(str);
		//checking highest level element in book is better choise, but later
		return validateFilename(name,FMBOOK) && F_StrIPrefix(name,(StringT)"book");
	case DRL:
		return F_StrISuffix(str,(StringT)".drl")
			&& !F_StrISuffix(str,(StringT)".backup.drl");
	default:
		return False;
	}
	////move to the end
	//while (*str)
	//{
	//	*str++;
	//}
	//str--;
	//while ((*str)&&(*str != '.'))
	//{
	//	*str--;
	//}
	//if (*str)
	//{
	//	switch (type)
	//	{
	//	case FM:
	//		if (F_StrIEqual((StringT)str,(StringT)".fm"))
	//		{
	//			*str--;
	//			while ((*str)&&(*str != '.'))
	//			{
	//				*str--;
	//			}
	//			if (*str == 0)
	//			{
	//				return True;
	//			}
	//			else
	//			{
	//				return (!F_StrIEqual((StringT)str,(StringT)".backup.fm"))&&(!F_StrIEqual((StringT)str,(StringT)".recover.fm"));
	//			}
	//		}
	//		else
	//		{
	//			return False;
	//		}
	//		break;
	//	case DRL:
	//		return (F_StrIEqual((StringT)str,(StringT)".drl"));
	//		break;
	//	case FMBOOK:
	//		*str--;
	//		while ((*str)&&(*str != '\\')&&(*str != '.'))
	//		{
	//			*str--;
	//		}
	//		if ((!*str)||(*str == '\\'))
	//		{
	//			return (F_StrISuffix(str,(StringT)".book"));
	//		}
	//		return False;
	//		break;
	//	case GENBOOK:
	//		*str--;
	//		while ((*str)&&(*str != '\\')&&(*str != '.'))
	//		{
	//			*str--;
	//		}
	//		if (!*str)
	//		{
	//			F_Printf(NULL,"Filename error");
	//			return False;
	//		}
	//		if (*str == '.')
	//		{
	//			return False;
	//		}
	//		*str++;
	//		return F_StrIPrefix(str,(StringT)"book") && F_StrISuffix(str,(StringT)".book");
	//		//this condition is more global - need to exclude files like book_foo.book
	//	default:
	//		F_ApiAlert("Invalid validation type",FF_ALERT_CONTINUE_NOTE);
	//		return False;
	//	}
	//}
	//else
	//{
	//	return False;
	//}
}

F_ObjHandleT openMainBook(StringT path)
{
	FilePathT *file;
	DirHandleT handle;
	IntT statusp;
	BoolT bookExists;
	StringT tmpBookPath, bookPath, tmpPath;
	F_ObjHandleT bookID, docID;

	handle = F_FilePathOpenDir(F_PathNameToFilePath (path, NULL, FDosPath), &statusp);
	if (!handle) 
	{
		F_Printf(NULL,"Handle Error0\n");
		writeToChannel("Error. Handle error.\n");
		return 0;
	}
	bookExists = False;
	//searching for main book
	while ((file = F_FilePathGetNext(handle, &statusp)) != NULL)
	{
		tmpBookPath = F_FilePathToPathName(file, FDosPath);
		tmpBookPath = fileFileName(tmpBookPath);
		if (F_StrIEqual(tmpBookPath,defaultBookName))
		{
			bookExists = True;
		}
		else if (F_StrSuffix(tmpBookPath,".fm.lck"))
		{
			F_DeleteFile(file);
		}
	} 
	F_FilePathCloseDir(handle);
	bookPath = F_StrCopyString(path);
	bookPath = F_Realloc(bookPath,F_StrLen(path)+F_StrLen(defaultBookName)+1,NO_DSE);
	F_StrCat(bookPath,defaultBookName);
	if (!bookExists)
	{
		bookID = F_ApiSimpleOpen("C:\\Program Files\\Adobe\\FrameMaker8\\Structure\\xml\\docline\\docline_book_template.book",False);
		handle = F_FilePathOpenDir(F_PathNameToFilePath(path,NULL,FDosPath),&statusp);
		if (!handle)
		{
			F_Printf(NULL,"Handle error 1\n");
			F_Free(&statusp);
			F_Free(&bookExists);
			F_ApiDeallocateString(&tmpBookPath);
			F_ApiDeallocateString(&bookPath);
			F_Free(&handle);
			F_FilePathFree(file);
			return 0;
		}
		while (file = F_FilePathGetNext(handle,&statusp))
		{
			tmpPath = F_FilePathToPathName(file,FDosPath);
			if ((validateFilename(tmpPath,FMBOOK))&&(!F_StrSuffix(tmpPath,defaultBookName)))
			{
				docID = F_ApiSimpleOpen(tmpPath,False);
				addStructuredElementToBook(docID,bookID,F_ApiGetId(FV_SessionId,bookID,FP_HighestLevelElement),
					F_ApiGetId(FV_SessionId,docID,FP_HighestLevelElement));
				F_ApiClose(docID,FF_CLOSE_MODIFIED);
			}
		}
		F_FilePathCloseDir(handle);
		F_ApiSimpleSave(bookID,bookPath,False);
	}
	else
	{
		bookID = F_ApiSimpleOpen(bookPath,False);
	}

	F_Free(&statusp);
	F_Free(&bookExists);
	F_Free(&docID);
	F_ApiDeallocateString(&tmpPath);
	F_ApiDeallocateString(&tmpBookPath);
	F_ApiDeallocateString(&bookPath);
	F_Free(&handle);
	F_FilePathFree(file);

	return bookID;
}
StringT getPlace(StringT fileName)
{
	if ((F_StrIEqual(fileName,(StringT)"InfElement")) ||
		(F_StrPrefix(fileName,(StringT)"InfProduct")))
	{
		return (StringT)"DocumentationCore";
	}
	else if (F_StrIEqual(fileName,(StringT)"FinalInfProduct"))
	{
		return (StringT)"ProductDocumentation";
	}
	else if (F_StrIEqual(fileName,(StringT)"Product"))
	{
		return (StringT)"ProductLine";
	}
	else
	{
		return (StringT)"";
	}
}
F_ObjHandleT componentExists(F_ObjHandleT bookID, F_ObjHandleT cID, StringT name)
{
	F_ObjHandleT compID, elemID;

	compID = F_ApiGetId(bookID,cID,FP_FirstChildElement);
	while (compID)
	{
		elemID = F_ApiGetId(bookID,compID,FP_ElementDef);
		if (F_StrIEqual(F_ApiGetString(bookID,elemID,FP_Name),name))
		{
			F_Free(&elemID);
			return compID;
		}
		else
		{
			compID = F_ApiGetId(bookID,compID,FP_NextSiblingElement);
		}
	}

	F_Free(&elemID);
	F_Free(&compID);

	return 0;
}

VoidT pathFilename(UCharT *str)
{
	while (*str)
	{
		*str++;
	}
	str--;
	while ((*str)&&(*str != '\\'))
	{
		*str = 0;
		*str--;
	}
}
StringT fileFileName(UCharT *str)
{
	while (*str)
	{
		*str++;
	}
	str--;
	while ((*str)&&(*str != '\\'))
	{
		*str--;
	}
	*str++;
	return str;
}
BoolT isDocLine(F_ObjHandleT docID)
{
	StringT elemName;

	elemName = F_ApiGetString(docID,F_ApiGetId(docID,F_ApiGetId(docID,F_ApiGetId(FV_SessionId,docID,FP_MainFlowInDoc),FP_HighestLevelElement),FP_ElementDef),FP_Name);
	return F_StrIEqual(elemName,(StringT)"InfElement") 
		|| F_StrIEqual(elemName,(StringT)"InfProduct")
		|| F_StrIEqual(elemName,(StringT)"DirTemplate")
		|| F_StrIEqual(elemName,(StringT)"Dictionary")
		|| F_StrIEqual(elemName,(StringT)"Directory")
		|| F_StrIEqual(elemName,(StringT)"FinalInfProduct")
		|| F_StrIEqual(elemName,(StringT)"Product");
}

StringT getHighestString(F_ObjHandleT docID)
{
	if (!docID)
	{
		F_Printf(NULL,"getHighestString:\n\tError: Null document\n");
		writeToChannel("Error. Document not opened.\n");
		return F_StrCopyString("");
	}
	return F_ApiGetString(docID,F_ApiGetId(docID,F_ApiGetId(docID,F_ApiGetId(FV_SessionId,docID,FP_MainFlowInDoc),FP_HighestLevelElement),FP_ElementDef),FP_Name);
}
VoidT addStructuredElementToBook(F_ObjHandleT bookID, F_ObjHandleT newBookID, F_ObjHandleT currElemID, F_ObjHandleT elemID)
{
	F_ElementLocT loc;
	F_ObjHandleT childID, parentID;
	F_AttributesT attrs;

	loc.childId = 0;
	loc.offset = 0;
	loc.parentId = currElemID;
	if (F_ApiGetObjectType(bookID,elemID) == FO_BookComponent)
	{
		F_ApiNewBookComponentInHierarchy(newBookID,F_ApiGetString(bookID,elemID,FP_Name),&loc);
	}
	else if (F_ApiGetObjectType(bookID,elemID) == FO_Element)
	{
		parentID = F_ApiGetNamedObject(newBookID,FO_ElementDef,F_ApiGetString(bookID,F_ApiGetId(bookID,elemID,FP_ElementDef),FP_Name));
		parentID = F_ApiNewElementInHierarchy(newBookID,parentID,&loc);
		attrs = F_ApiGetAttributes(bookID,elemID);
		F_ApiSetAttributes(newBookID,parentID,&attrs);
		childID = F_ApiGetId(bookID,elemID,FP_FirstChildElement);
		while (childID)
		{
			if (F_ApiGetId(bookID,childID,FP_BookComponent))
			{
				addStructuredElementToBook(bookID,newBookID,parentID,F_ApiGetId(bookID,childID,FP_BookComponent));
			}
			else
			{
				addStructuredElementToBook(bookID,newBookID,parentID,childID);
			}
			childID = F_ApiGetId(bookID,childID,FP_NextSiblingElement);
		}
	}
	else
	{
		F_ApiAlert("Error in adding to hierarchy",FF_ALERT_CONTINUE_NOTE);
	}
}
IntT getActiveBookID()
{
	IntT bookID;
	StringT dirPath, path;

	bookID = F_ApiGetId(0,FV_SessionId,FP_ActiveBook);
	//Active document is not framemaker book
	if (!bookID)
	{
		bookID = F_ApiGetId(0,FV_SessionId,FP_ActiveDoc);
		//Active document is not docline document
		if (!isDocLine(bookID))
		{
			return 0;
		}
		path = F_ApiGetString(FV_SessionId,bookID,FP_Name);
		pathFilename(path);
		dirPath = F_Alloc(F_StrLen(path)+F_StrLen(defaultBookName)+5,NO_DSE);
		F_Sprintf(dirPath,"%s%s",path,defaultBookName);
		bookID = F_ApiSimpleOpen(dirPath,False);
	}
	//Active document is not docline book
	else if (!F_StrISuffix(F_ApiGetString(FV_SessionId,bookID,FP_Name),defaultBookName))
	{
		return 0;
	}
	//Active document is docline book
	F_ApiDeallocateString(&path);
	F_ApiDeallocateString(&dirPath);
	return bookID;
}
VoidT closeAllDocs()
{
	F_ObjHandleT docID;
	docID = F_ApiGetId(0,FV_SessionId,FP_FirstOpenDoc);
	while(docID)
	{
		F_ApiClose(docID,FF_CLOSE_MODIFIED);
		docID = F_ApiGetId(0,FV_SessionId,FP_NextOpenDocInSession);
	}
}

StringT getMainBookName(F_ObjHandleT docID)
{
	StringT pathName, newPathName;

	pathName = F_ApiGetString(FV_SessionId, docID, FP_Name);
	pathFilename(pathName);
	newPathName = F_Alloc(F_StrLen(pathName)+F_StrLen(defaultBookName)+5,NO_DSE);
	F_Sprintf(newPathName,"%s%s",pathName,defaultBookName);
	return newPathName;
}

BoolT setDefaultDirectory(StringT dirPath)
{
	F_ApiSetString(0,FV_SessionId,FP_OpenDir, dirPath);
	return TRUE;
}

BoolT cleanTempDirectory()
{
	DirHandleT handle;
	IntT statusp, i;
	UCharT buf[100];
	StringT tmpDirPath;
	FilePathT *tmpDirFilepath, *file;

	tmpDirPath = F_StrCopyString(F_ApiClientDir());
	i = F_Sprintf(buf,"%s",tmpDirPath);
	i = F_Sprintf(buf+i,"\\docline\\temp\\");
	tmpDirPath = F_StrCopyString((StringT)buf);
	tmpDirFilepath = F_PathNameToFilePath(tmpDirPath,NULL,FDosPath);
	handle = F_FilePathOpenDir(tmpDirFilepath, &statusp);
	if (!handle)
	{
		F_Printf(NULL,"%s\n","CleanTempDirectory.DirHandle error.");
		writeToChannel("CleanTempDirectory.DirHandle error.\n");
		return FALSE;
	}
	while (file = F_FilePathGetNext(handle,&statusp))
	{
		F_DeleteFile(file);
	}

	return TRUE;
}