//
//  SwDisplayWindow.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/2/14.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef SwDisplayWindow_h
#define SwDisplayWindow_h

enum CSwDisplayWindowPutDataFlag {
    SDWPDF_MIRROR   = 0x01,
    SDWPDF_COPY     = 0x02,
};

class CSwDisplayWindow
{
#pragma mark - public functions
public:
    CSwDisplayWindow(void* _UIImageView);
    virtual ~CSwDisplayWindow();

public:
    int PutData(const char* data, int flags);
    void Mirror(bool mirror=true);

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    void* m_UIImageView;
//    void* m_DisplayImage;
    void* m_glLayer;
    bool m_bForceMirror;
};

#endif /* SwDisplayWindow_h */
