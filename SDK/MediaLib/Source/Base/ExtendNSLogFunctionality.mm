//
//  ExtendNSLogFunctionality.m
//  SwIMediaLib
//
//  Created by 李招华 on 16/3/31.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#import <Foundation/Foundation.h>

#ifdef NSLog
#undef NSLog
#endif

void ExtendNSLog(const char *file, int lineNumber, const char *functionName, NSString *format, ...)
{
#ifdef DEBUG
    NSLog(@"_");
#endif
    // Type to hold information about variable arguments.
    va_list ap;
    // Initialize a variable argument list.
    va_start (ap, format);
    // NSLog only adds a newline to the end of the NSLog format if
    // one is not already there.
    // Here we are utilizing this feature of NSLog()
    if (![format hasSuffix: @"\n"])
    {
        format = [format stringByAppendingString: @"\n"];
    }
    NSString *body = [[NSString alloc] initWithFormat:format arguments:ap];
    // End using variable argument list.
    va_end (ap);
    NSString *fileName = [[NSString stringWithUTF8String:file] lastPathComponent];

#ifdef DEBUG
    fprintf(stderr, "(%s) (%s:%d) %s",
            functionName, [fileName UTF8String],
            lineNumber, [body UTF8String]);
#endif
}

void Log2File(const char *file, int lineNumber, const char *functionName, NSString *format, ...)
{
#ifdef DEBUG
    NSLog(@"_");
#endif
    // Type to hold information about variable arguments.
    va_list ap;
    // Initialize a variable argument list.
    va_start (ap, format);
    // NSLog only adds a newline to the end of the NSLog format if
    // one is not already there.
    // Here we are utilizing this feature of NSLog()
    if (![format hasSuffix: @"\n"])
    {
        format = [format stringByAppendingString: @"\n"];
    }
    NSString *body = [[NSString alloc] initWithFormat:format arguments:ap];
    // End using variable argument list.
    va_end (ap);
    NSString *fileName = [[NSString stringWithUTF8String:file] lastPathComponent];

#ifdef DEBUG
    fprintf(stderr, "(%s) (%s:%d) %s",
            functionName, [fileName UTF8String],
            lineNumber, [body UTF8String]);
#endif

    NSString* nspathDocument = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
    if (!nspathDocument) {
        return;
    }

    NSString* nspathLog = [nspathDocument stringByAppendingString:@"/SwIMediaLib.log"];
    FILE* fileWriteTo = fopen([nspathLog UTF8String], "a");
    if (!fileWriteTo) {
        return;
    }
    fseek(fileWriteTo, 0, SEEK_END);
    long fileSize = ftell(fileWriteTo);
    if (fileSize > 1024*1024) {
        fclose(fileWriteTo);

        NSString* nspathLogBak = [nspathDocument stringByAppendingString:@"/SwIMediaLib_old.log"];
        remove([nspathLogBak UTF8String]);
        rename([nspathLog UTF8String], [nspathLogBak UTF8String]);

        fileWriteTo = fopen([nspathLog UTF8String], "a");
        if (!fileWriteTo) {
            return;
        }
    }

    fprintf(fileWriteTo, "(%s) (%s:%d) %s",
            functionName, [fileName UTF8String],
            lineNumber, [body UTF8String]);
    fclose(fileWriteTo);
}