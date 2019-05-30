//----------------------------------------------------
// file: SceneObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SceneObject.h"
#include "scene.h"

//----------------------------------------------------

void SceneObject::Select( BOOL flag ){
    if (m_Visible&&(m_Selected!=flag)){
        m_Selected = flag;
        UI->RedrawScene();
//		Scene->UndoSave();
    }
};

void SceneObject::Show( BOOL flag ){
    m_Visible = flag;
    if (!m_Visible) m_Selected = false;
    UI->RedrawScene();
};

void SceneObject::Lock( BOOL flag ){
    m_Locked = flag;
};

//----------------------------------------------------

