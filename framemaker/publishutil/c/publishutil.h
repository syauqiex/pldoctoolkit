#ifndef PUBLISHUTIL_H
#define PUBLISHUTIL_H

/* calls java impementation of export util 
   use empty string for default value */
int callJavaPublishUtil(char *jarPath, // path to a jar, containing java class
				 	    char *srcDir,  // path to a directory, containing all drl files
					    char *scrFile, // name of the file, containgn final inf product
					    char *srcId,   // id of final inf product in file
					    char *format,  // format of file to be exported to
					    char *dstFile);// destination file name

int callJavaImportUtil(char *jarPath,  // path to a jar, containing java class
					   char *srcDir);  // path to a directory, containing all drl files

int callJavaExportUtil(char *jarPath,  // path to a jar, containing java class
					   char *srcDir);  // path to a directory, containing all drl files

#endif /* PUBLISHUTIL_H */