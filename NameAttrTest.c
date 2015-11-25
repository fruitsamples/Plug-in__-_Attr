/* this is box, modified to be a test for the name plug-in attribute */// NameAttrTest.c - QuickDraw 3d routines//// This is box, the QuickDraw 3D starter program.  Written for the// Getting started with QuickDraw 3D Develop article.  This app does not have // graceful error handling - it's purpose is to illustrate a very basic QuickDraw // 3D program.//// Nick Thompson, Robert Dierkes - April 24, 1997// // �1994-97 Apple computer Inc., All Rights Reserved//// system headers#include <Devices.h>#include <Dialogs.h>#include <DiskInit.h>#include <Fonts.h>#include <Menus.h>#include <PictUtils.h>#include <QDOffScreen.h>#include <QuickDraw.h>#include <SegLoad.h>#include <StandardFile.h>#include <TextEdit.h>#include <strings.h>#include <string.h>// for QuickDraw 3D#include "QD3D.h"#include "QD3DAcceleration.h"#include "QD3DCamera.h"#include "QD3DDrawContext.h"#include "QD3DGeometry.h"#include "QD3DGroup.h"#include "QD3DLight.h"#include "QD3DMath.h"#include "QD3DPick.h"#include "QD3DRenderer.h"#include "QD3DSet.h"#include "QD3DShader.h"#include "QD3DTransform.h"#include "NameAttribute.h"//-------------------------------------------------------------------------------------------struct _documentRecord {	TQ3ViewObject	fView ;					// the view for the scene	TQ3GroupObject	fModel ;				// object in the scene being modelled	TQ3StyleObject	fInterpolation ;		// interpolation style used when rendering	TQ3StyleObject	fBackFacing ;			// whether to draw shapes that face away from the camera	TQ3StyleObject	fFillStyle ;			// whether drawn as solid filled object or decomposed to components	TQ3Matrix4x4		fRotation;			// the transform for the model};typedef struct _documentRecord DocumentRec, *DocumentPtr, **DocumentHdl ;//-------------------------------------------------------------------------------------------// function prototypesvoid InitDocumentData( 	DocumentPtr theDocument ) ;void DisposeDocumentData( 	DocumentPtr theDocument ) ;void 	SubmitDocumentData( 	DocumentPtr theDocument ) ;TQ3Status DocumentDraw3DData( 	DocumentPtr theDocument ) ;short HiWrd(	long aLong) ;short LoWrd(	long aLong ) ;void InitToolbox(	void ) ;void PickAndProcess( 	WindowPtr 	window,	DocumentPtr	theDocument, 	Point 		mouseLoc ) ;void ChangeObjectName(	TQ3GeometryObject	objectPicked,	TQ3AttributeSet		attribSet) ;void MainEventLoop(	void ) ;void HandleKeyPress(	EventRecord *event ) ;TQ3ViewObject MyNewView(	WindowPtr theWindow ) ;TQ3DrawContextObject MyNewDrawContext(	WindowPtr theWindow ) ;TQ3CameraObject MyNewCamera(	WindowPtr theWindow) ;TQ3GroupObject MyNewLights(	void ) ;void MyColorBoxFaces( 	TQ3BoxData *myBoxData ) ;TQ3GroupPosition MyAddTransformedObjectToGroup( 	TQ3GroupObject theGroup, 	TQ3Object theObject, 	TQ3Vector3D *translation ) ;TQ3GroupObject MyNewModel(	void ) ;//-------------------------------------------------------------------------------------------//Boolean 		gQuitFlag 		= false ;WindowPtr		gMainWindow		= nil ;DocumentRec		gDocument ;//-------------------------------------------------------------------------------------------// main()// entry point for the application, initialize the toolbox, initialize QuickDraw 3D// and enter the main event loop.  On exit from the main event loop, we want to call// the QuickDraw 3D exit function to clean up QuickDraw 3d.void main(void){	TQ3Status	myStatus;	Rect		rBounds = { 50, 50, 450, 450 } ;	Str255		title = "\pName Attributes on Geometries" ;	InitToolbox() ;	//	Initialize QuickDraw 3D, open a connection to the QuickDraw 3D library	myStatus = Q3Initialize();	if ( myStatus == kQ3Failure )	{		DebugStr("\pQ3Initialize returned failure.  Use QD3D debug version to find out why.");			ExitToShell() ;	}		// set up our globals	gQuitFlag = false ;	gMainWindow = NewCWindow(nil,&rBounds,title,true,noGrowDocProc,(WindowPtr)-1,true,0) ;	InitDocumentData( &gDocument ) ;		MainEventLoop();		DisposeDocumentData( &gDocument ) ;		//	Close our connection to the QuickDraw 3D library	myStatus = Q3Exit();		if ( myStatus == kQ3Failure )		DebugStr("\pQ3Exit returned failure.  Use debug version to find out why");		ExitToShell() ;}//-------------------------------------------------------------------------------------------//void InitDocumentData( DocumentPtr theDocument ) {	// sets up the 3d data for the scene	// Create view for QuickDraw 3D.	theDocument->fView = MyNewView( (WindowPtr)gMainWindow ) ;	// the main display group:	theDocument->fModel = MyNewModel() ;	// the drawing styles:	theDocument->fInterpolation = Q3InterpolationStyle_New(kQ3InterpolationStyleNone) ;	theDocument->fBackFacing = Q3BackfacingStyle_New(kQ3BackfacingStyleBoth ) ;	theDocument->fFillStyle = Q3FillStyle_New(kQ3FillStyleFilled ) ;	// set the rotation matrix the identity matrix	Q3Matrix4x4_SetIdentity(&theDocument->fRotation);}//-----------------------------------------------------------------------------// void DisposeDocumentData( DocumentPtr theDocument){	Q3Object_Dispose(theDocument->fView) ;				// the view for the scene	Q3Object_Dispose(theDocument->fModel) ;				// object in the scene being modelled	Q3Object_Dispose(theDocument->fInterpolation) ;		// interpolation style used when rendering	Q3Object_Dispose(theDocument->fBackFacing) ;		// whether to draw shapes that face away from the camera	Q3Object_Dispose(theDocument->fFillStyle) ;			// whether drawn as solid filled object or decomposed to components}//-----------------------------------------------------------------------------// void 	SubmitDocumentData( 	DocumentPtr theDocument ) {	Q3Style_Submit( theDocument->fInterpolation, theDocument->fView );	Q3Style_Submit( theDocument->fBackFacing, theDocument->fView );	Q3Style_Submit( theDocument->fFillStyle, theDocument->fView );	Q3MatrixTransform_Submit( &theDocument->fRotation, theDocument->fView );	Q3DisplayGroup_Submit( theDocument->fModel, theDocument->fView );}//-----------------------------------------------------------------------------// TQ3Status DocumentDraw3DData( DocumentPtr theDocument ){		Q3View_StartRendering(theDocument->fView );	do {		SubmitDocumentData( theDocument ) ;	} while (Q3View_EndRendering(theDocument->fView) == kQ3ViewStatusRetraverse );	Q3View_Sync( theDocument->fView ) ;	return kQ3Success ;}//-------------------------------------------------------------------------------------------//short HiWrd(long aLong){	return	(((aLong) >> 16) & 0xFFFF) ;}//-------------------------------------------------------------------------------------------//short LoWrd(long aLong){	return	((aLong) & 0xFFFF) ;}//-------------------------------------------------------------------------------------------//void InitToolbox(){	Handle		menuBar = nil;	MaxApplZone() ;	MoreMasters() ; MoreMasters() ; MoreMasters() ; 		InitGraf( &qd.thePort );	InitFonts();	InitWindows();	InitCursor();	FlushEvents( everyEvent, 0 ) ;		/* initialize application globals */	gQuitFlag = false;	}//-------------------------------------------------------------------------------------------//void PickAndProcess( 	WindowPtr 	window,	DocumentPtr	theDocument, 	Point 		mouseLoc ){	TQ3WindowPointPickData	myWindowPointPickData;	TQ3GeometryObject		objectPicked;	TQ3PickObject			pickObject = NULL ;	unsigned long			numPicked;	unsigned long			i;	TQ3AttributeSet 		attribSet;	/* set up the picking data field */	myWindowPointPickData.data.sort = kQ3PickSortNearToFar;	myWindowPointPickData.data.mask = kQ3PickDetailMaskObject;	myWindowPointPickData.data.numHitsToReturn = kQ3ReturnAllHits;	/* get the point of the click in local (window) co-ordinates */	GlobalToLocal( &mouseLoc ) ;	myWindowPointPickData.point.x = mouseLoc.h ;	myWindowPointPickData.point.y = mouseLoc.v  ;	myWindowPointPickData.vertexTolerance = 3.0 ;	myWindowPointPickData.edgeTolerance	  = 3.0 ;	if ((pickObject = Q3WindowPointPick_New( &myWindowPointPickData ))  != NULL) {		Q3View_StartPicking(theDocument->fView, pickObject);		do {			SubmitDocumentData(theDocument) ;		} while (Q3View_EndPicking(theDocument->fView) == kQ3ViewStatusRetraverse );	}	if (Q3Pick_GetNumHits(pickObject, &numPicked) == kQ3Success) {		/* Get all geometries hit */		for (i = 0; i < numPicked; i++) {			if (Q3Pick_GetPickDetailData(					pickObject, 					i, 					kQ3PickDetailMaskObject, 					&objectPicked) == kQ3Failure) {				break;			}			/* switch on/off highlighting for this object */			Q3Geometry_GetAttributeSet(objectPicked, &attribSet);			if (!attribSet) {				attribSet = Q3AttributeSet_New();				Q3Geometry_SetAttributeSet(objectPicked, attribSet);			}			/* Set or get the name of the geometry hit */			ChangeObjectName(objectPicked, attribSet);			Q3Object_Dispose(attribSet);			Q3Object_Dispose(objectPicked);		}		DocumentDraw3DData( theDocument ) ;	}	Q3Object_Dispose(pickObject);	 }//-------------------------------------------------------------------------------------------//void ChangeObjectName(	TQ3GeometryObject	objectPicked,	TQ3AttributeSet		attribSet){	DialogPtr	theDialog ;	Handle		itemHndl = nil;	Rect		itemRect ;	short		itemType ;	short		itemHit ;	TQ3Switch	highlightSwitch ;	char		nameString[255] ;	if ( NameAttribute_GetName( objectPicked, nameString) == kQ3Success &&		 *nameString != '\0') {		c2pstr(nameString);		ParamText((unsigned char *)nameString, (unsigned char *)"\0",  (unsigned char *)"\0",  (unsigned char *)"\0" ) ;		Alert( 128, nil ) ;		/* add the hightlight attribute it's off now */		highlightSwitch = kQ3Off ;		Q3AttributeSet_Add(attribSet, 						   kQ3AttributeTypeHighlightState,						   &highlightSwitch);	}	else {		/* add the hightlight attribute it is on now*/		highlightSwitch = kQ3On ;		Q3AttributeSet_Add(attribSet, 						   kQ3AttributeTypeHighlightState,						   &highlightSwitch);		theDialog = GetNewDialog(128, nil, (WindowPtr)-1) ;		do {			ModalDialog(nil, &itemHit);		} while( itemHit != 1 ) ;		GetDialogItem(theDialog, 4, &itemType, (Handle*)&itemHndl, &itemRect);		GetDialogItemText(itemHndl, (StringPtr) nameString);		DisposeDialog( theDialog ) ;		if( itemHndl != nil && objectPicked != nil )		{			p2cstr((StringPtr) nameString) ;			NameAttribute_SetName( objectPicked, (char *)nameString) ;		}				   		DisposeHandle( (Handle)itemHndl ) ;	}}//-------------------------------------------------------------------------------------------//void MainEventLoop(){	EventRecord 	event;	WindowPtr   	window;	short       	thePart;	Rect        	screenRect, updateRect;	Point			aPoint = {100, 100};	CGrafPtr		savedPort ;		while( !gQuitFlag )	{		if (WaitNextEvent( everyEvent, &event, 10, nil ))		{			switch (event.what) {				case mouseDown:					thePart = FindWindow( event.where, &window );					switch( thePart ) {						case inMenuBar: 							break;						case inDrag:							screenRect = (**GetGrayRgn()).rgnBBox;							DragWindow( window, event.where, &screenRect );							break ;						case inContent:							if (window != FrontWindow())								SelectWindow( window );							else								PickAndProcess( window, &gDocument, event.where ) ;							break ;						case inGoAway:							if (TrackGoAway( window, event.where )) {								DisposeWindow ( window );								gQuitFlag = true;							}							break ;													default:							break ;					}					break ;																	case updateEvt:									window = (WindowPtr)event.message;					updateRect = (**(window->visRgn)).rgnBBox;					SetPort( window ) ;					BeginUpdate( window );					DocumentDraw3DData( &gDocument ) ;					EndUpdate( window );					break ;									case keyDown:				case autoKey:					HandleKeyPress(&event);					break;									case diskEvt:					if ( HiWrd(event.message) != noErr ) 						(void) DIBadMount(aPoint, event.message);					break;									case osEvt:				case activateEvt:					break;			}		}		else {			// we received a null event, rotate the cube			TQ3Matrix4x4	tmp;			Rect			theRect = ((GrafPtr)gMainWindow)->portRect ;						SetPort((GrafPtr)gMainWindow) ;			Q3Matrix4x4_SetRotate_XYZ(&tmp, 0.1, 0.12, 0.08);			Q3Matrix4x4_Multiply(&gDocument.fRotation, &tmp, &gDocument.fRotation);			InvalRect( &theRect ) ;		}	}}//-------------------------------------------------------------------------------------------//void HandleKeyPress(	EventRecord *event){}//-------------------------------------------------------------------------------------------//TQ3ViewObject MyNewView(	WindowPtr theWindow){	TQ3Status				myStatus;	TQ3ViewObject			myView;	TQ3DrawContextObject		myDrawContext;	TQ3RendererObject		myRenderer;	TQ3CameraObject			myCamera;	TQ3GroupObject			myLights;		myView = Q3View_New();		//	Create and set draw context.	if ((myDrawContext = MyNewDrawContext(theWindow)) == nil )		goto bail;	if ((myStatus = Q3View_SetDrawContext(myView, myDrawContext)) == kQ3Failure )		goto bail;	Q3Object_Dispose( myDrawContext ) ;		//	Create and set renderer.				// this would use the wireframe renderer#if 0	myRenderer = Q3Renderer_NewFromType(kQ3RendererTypeWireFrame);	if ((myStatus = Q3View_SetRenderer(myView, myRenderer)) == kQ3Failure ) {		goto bail;	}	#else	// this would use the interactive software renderer	if ((myRenderer = Q3Renderer_NewFromType(kQ3RendererTypeInteractive)) != nil ) {		if ((myStatus = Q3View_SetRenderer(myView, myRenderer)) == kQ3Failure ) {			goto bail;		}		// these two lines set us up to use the best possible renderer,		// including  hardware if it is installed.		Q3InteractiveRenderer_SetDoubleBufferBypass (myRenderer, kQ3True);		Q3InteractiveRenderer_SetPreferences(myRenderer, kQAVendor_BestChoice, 0);	}	else {		goto bail;	}#endif	Q3Object_Dispose( myRenderer ) ;		//	Create and set camera.	if ( (myCamera = MyNewCamera(theWindow)) == nil )		goto bail;	if ((myStatus = Q3View_SetCamera(myView, myCamera)) == kQ3Failure )		goto bail;	Q3Object_Dispose( myCamera ) ;		//	Create and set lights.	if ((myLights = MyNewLights()) == nil )		goto bail;	if ((myStatus = Q3View_SetLightGroup(myView, myLights)) == kQ3Failure )		goto bail;	Q3Object_Dispose(myLights);	//	Done!!!	return ( myView );	bail:	//	If any of the above failed, then don't return a view.	return ( nil );}//----------------------------------------------------------------------------------TQ3DrawContextObject MyNewDrawContext(	WindowPtr theWindow ){	TQ3DrawContextData		myDrawContextData;	TQ3MacDrawContextData	myMacDrawContextData;	TQ3ColorARGB			ClearColor;	TQ3DrawContextObject	myDrawContext ;		//	Set the background color.	ClearColor.a = 1.0;	ClearColor.r = 1.0;	ClearColor.g = 1.0;	ClearColor.b = 1.0;		//	Fill in draw context data.	myDrawContextData.clearImageMethod = kQ3ClearMethodWithColor;	myDrawContextData.clearImageColor = ClearColor;	myDrawContextData.paneState = kQ3False;	myDrawContextData.maskState = kQ3False;	myDrawContextData.doubleBufferState = kQ3True; 	myMacDrawContextData.drawContextData = myDrawContextData;		myMacDrawContextData.window = (CGrafPtr) theWindow;		// this is the window associated with the view	myMacDrawContextData.library = kQ3Mac2DLibraryNone;	myMacDrawContextData.viewPort = nil;	myMacDrawContextData.grafPort = nil;		//	Create draw context and return it, if it�s nil the caller must handle	myDrawContext = Q3MacDrawContext_New(&myMacDrawContextData) ;	return myDrawContext ;}//----------------------------------------------------------------------------------TQ3CameraObject MyNewCamera(	WindowPtr theWindow){	TQ3ViewAngleAspectCameraData	perspectiveData;	TQ3CameraObject				camera;		TQ3Point3D 					from 	= { 0.0, 0.0, 7.25 };	TQ3Point3D 					to 		= { 0.0, 0.0, 0.0 };	TQ3Vector3D 				up 		= { 0.0, 1.0, 0.0 };	float 						fieldOfView = 1.0;	float 						hither 		= 0.1;	float 						yon 		= 10;		TQ3Status					returnVal = kQ3Failure ;	perspectiveData.cameraData.placement.cameraLocation 	= from;	perspectiveData.cameraData.placement.pointOfInterest 	= to;	perspectiveData.cameraData.placement.upVector 			= up;	perspectiveData.cameraData.range.hither	= hither;	perspectiveData.cameraData.range.yon 	= yon;	perspectiveData.cameraData.viewPort.origin.x = -1.0;	perspectiveData.cameraData.viewPort.origin.y = 1.0;	perspectiveData.cameraData.viewPort.width = 2.0;	perspectiveData.cameraData.viewPort.height = 2.0;		perspectiveData.fov				= fieldOfView;	perspectiveData.aspectRatioXToY	=		(float) (theWindow->portRect.right - theWindow->portRect.left) / 		(float) (theWindow->portRect.bottom - theWindow->portRect.top);	camera = Q3ViewAngleAspectCamera_New(&perspectiveData);	return camera ;}//----------------------------------------------------------------------------------TQ3GroupObject MyNewLights(	void ){	TQ3GroupPosition		myGroupPosition;	TQ3GroupObject			myLightList;	TQ3LightData			myLightData;	TQ3PointLightData		myPointLightData;	TQ3DirectionalLightData	myDirectionalLightData;	TQ3LightObject			myAmbientLight, myPointLight, myFillLight;	TQ3Point3D				pointLocation = { -10.0, 0.0, 10.0 };	TQ3Vector3D				fillDirection = { 10.0, 0.0, 10.0 };	TQ3ColorRGB				WhiteLight = { 1.0, 1.0, 1.0 };		//	Set up light data for ambient light.  This light data will be used for point and fill	//	light also.	myLightData.isOn = kQ3True;	myLightData.color = WhiteLight;		//	Create ambient light.	myLightData.brightness = .2;	myAmbientLight = Q3AmbientLight_New(&myLightData);	if ( myAmbientLight == nil )		goto bail;		//	Create point light.	myLightData.brightness = 1.0;	myPointLightData.lightData = myLightData;	myPointLightData.castsShadows = kQ3False;	myPointLightData.attenuation = kQ3AttenuationTypeNone;	myPointLightData.location = pointLocation;	myPointLight = Q3PointLight_New(&myPointLightData);	if ( myPointLight == nil )		goto bail;	//	Create fill light.	myLightData.brightness = .2;	myDirectionalLightData.lightData = myLightData;	myDirectionalLightData.castsShadows = kQ3False;	myDirectionalLightData.direction = fillDirection;	myFillLight = Q3DirectionalLight_New(&myDirectionalLightData);	if ( myFillLight == nil )		goto bail;	//	Create light group and add each of the lights into the group.	myLightList = Q3LightGroup_New();	if ( myLightList == nil )		goto bail;	myGroupPosition = Q3Group_AddObject(myLightList, myAmbientLight);	if ( myGroupPosition == 0 )		goto bail;	myGroupPosition = Q3Group_AddObject(myLightList, myPointLight);	if ( myGroupPosition == 0 )		goto bail;	myGroupPosition = Q3Group_AddObject(myLightList, myFillLight);	if ( myGroupPosition == 0 )		goto bail;	Q3Object_Dispose( myAmbientLight ) ;	Q3Object_Dispose( myPointLight ) ;	Q3Object_Dispose( myFillLight ) ;	//	Done!	return ( myLightList );	bail:	//	If any of the above failed, then return nothing!	return ( nil );}void MyColorBoxFaces( 	TQ3BoxData *myBoxData ){	TQ3ColorRGB				faceColor ;	short 					face ;		// sanity check - you need to have set up 	// the face attribute set for the box data 	// before calling this.		if( myBoxData->faceAttributeSet == NULL )		return ;	// make each face of a box a different color		for( face = 0; face < 6; face++) {		myBoxData->faceAttributeSet[face] = Q3AttributeSet_New();		switch( face ) {			case 0:				faceColor.r = 1.0;				faceColor.g = 0.0;				faceColor.b = 0.0;				break;			case 1:				faceColor.r = 0.0;				faceColor.g = 1.0;				faceColor.b = 0.0;				break;			case 2:				faceColor.r = 0.0;				faceColor.g = 0.0;				faceColor.b = 1.0;				break;			case 3:				faceColor.r = 1.0;				faceColor.g = 1.0;				faceColor.b = 0.0;				break;			case 4:				faceColor.r = 1.0;				faceColor.g = 0.0;				faceColor.b = 1.0;				break;			case 5:				faceColor.r = 0.0;				faceColor.g = 1.0;				faceColor.b = 1.0;				break;		}		Q3AttributeSet_Add(myBoxData->faceAttributeSet[face], kQ3AttributeTypeDiffuseColor, &faceColor);	}}TQ3GroupPosition MyAddTransformedObjectToGroup( 	TQ3GroupObject theGroup, 	TQ3Object theObject, 	TQ3Vector3D *translation ){	TQ3TransformObject	transform;	transform = Q3TranslateTransform_New(translation);	Q3Group_AddObject(theGroup, transform);		Q3Object_Dispose(transform);	return Q3Group_AddObject(theGroup, theObject);	}TQ3GroupObject MyNewModel(	void ){	TQ3GroupObject			myGroup = NULL;	TQ3GroupPosition		myGroupPosition;	TQ3ShaderObject			myIlluminationShader ;	TQ3BoxData				boxData;	TQ3ConeData				coneData;	TQ3CylinderData			cylinderData;	TQ3TorusData			torusData;	TQ3GeometryObject		boxGeo,							coneGeo,							cylinderGeo,							torusGeo;	short					face ;	TQ3SetObject			boxFaces[6] ;	TQ3ColorRGB				color;	TQ3Vector3D				translation;	// Create a group for the complete model.	// do not use Q3OrderedDisplayGroup_New since in this	// type of group all of the translations are applied before	// the objects in the group are drawn, in this instance we 	// dont want this.	if ((myGroup = Q3DisplayGroup_New()) != NULL ) {		// Define a shading type for the group		// and add the shader to the group		myIlluminationShader = Q3PhongIllumination_New();		if (myIlluminationShader != NULL ) {			Q3Group_AddObject(myGroup, myIlluminationShader);			Q3Object_Dispose(myIlluminationShader);			}			// create the box		// set up the colored faces for the box data		boxData.faceAttributeSet = boxFaces;		boxData.boxAttributeSet = nil;		MyColorBoxFaces( &boxData ) ;		Q3Point3D_Set(&boxData.origin, 0, 0, 0);		Q3Vector3D_Set(&boxData.orientation, 0, 1.25, 0);		Q3Vector3D_Set(&boxData.majorAxis, 0, 0, 2);			Q3Vector3D_Set(&boxData.minorAxis, 1.25, 0, 0);			boxGeo = Q3Box_New(&boxData);		// create the cone		Q3Point3D_Set(&coneData.origin, 0, 0, 0);		Q3Vector3D_Set(&coneData.orientation, 0, 1.75, 0);		Q3Vector3D_Set(&coneData.majorRadius, 0, 0, 0.75);			Q3Vector3D_Set(&coneData.minorRadius, 0.75, 0, 0);		coneData.uMin = 0.0;		coneData.uMax = 1.0;		coneData.vMin = 0.0;		coneData.vMax = 1.0;		coneData.caps = kQ3EndCapMaskBottom;		coneData.interiorAttributeSet 	= NULL;		coneData.faceAttributeSet 		= NULL;		coneData.bottomAttributeSet		= NULL;		coneData.coneAttributeSet = Q3AttributeSet_New();		if (coneData.coneAttributeSet != NULL ) {			Q3ColorRGB_Set(&color, 0.3, 0.9, 0.9);			Q3AttributeSet_Add(coneData.coneAttributeSet, kQ3AttributeTypeDiffuseColor, &color);		}		coneGeo = Q3Cone_New(&coneData);		// create the cylinder		Q3Point3D_Set(&cylinderData.origin, 0, 0, 0);		Q3Vector3D_Set(&cylinderData.orientation, 0, 1.25, 0);		Q3Vector3D_Set(&cylinderData.majorRadius, 0, 0, 0.75);			Q3Vector3D_Set(&cylinderData.minorRadius, 0.75, 0, 0);		cylinderData.uMin = 0.0;		cylinderData.uMax = 1.0;		cylinderData.vMin = 0.0;		cylinderData.vMax = 1.0;		cylinderData.caps = kQ3EndCapMaskTop | kQ3EndCapMaskBottom;		cylinderData.interiorAttributeSet 	= NULL;		cylinderData.topAttributeSet 		= NULL;		cylinderData.faceAttributeSet 		= NULL;		cylinderData.bottomAttributeSet		= NULL;		cylinderData.cylinderAttributeSet = Q3AttributeSet_New();		if (cylinderData.cylinderAttributeSet != NULL ) {			Q3ColorRGB_Set(&color, 0.9, 0.3, 0.9);			Q3AttributeSet_Add(cylinderData.cylinderAttributeSet, kQ3AttributeTypeDiffuseColor, &color);		}		cylinderGeo = Q3Cylinder_New(&cylinderData);		// create the torus		Q3Point3D_Set(&torusData.origin, 0, 0.8, 0);		Q3Vector3D_Set(&torusData.orientation, 0, 0.4, 0);		Q3Vector3D_Set(&torusData.majorRadius, 0, 0, 0.75);			Q3Vector3D_Set(&torusData.minorRadius, 0.75, 0, 0);		torusData.ratio = 1.0;		torusData.uMin = 0.0;		torusData.uMax = 1.0;		torusData.vMin = 0.0;		torusData.vMax = 1.0;		torusData.caps = kQ3EndCapNone;		torusData.interiorAttributeSet 	= NULL;		torusData.torusAttributeSet = Q3AttributeSet_New();		if (torusData.torusAttributeSet != NULL ) {			Q3ColorRGB_Set(&color, 1.0, 1.0, 0.0);			Q3AttributeSet_Add(torusData.torusAttributeSet, kQ3AttributeTypeDiffuseColor, &color);		}		torusGeo = Q3Torus_New(&torusData);		// add all geometries to group and translate them		Q3Vector3D_Set(&translation, 0, 0, 0);		MyAddTransformedObjectToGroup( myGroup, boxGeo, &translation ) ;		Q3Object_Dispose( boxGeo );		for( face = 0; face < 6; face++) {			if( boxData.faceAttributeSet[face] != NULL )				Q3Object_Dispose(boxData.faceAttributeSet[face]);		}		Q3Vector3D_Set(&translation, 2, 0, 0);		MyAddTransformedObjectToGroup( myGroup, coneGeo, &translation ) ;		Q3Object_Dispose(coneGeo);		if (coneData.coneAttributeSet != NULL )			Q3Object_Dispose(coneData.coneAttributeSet);		Q3Vector3D_Set(&translation, 0, 0, -2);		MyAddTransformedObjectToGroup( myGroup, cylinderGeo, &translation ) ;		Q3Object_Dispose(cylinderGeo);		if (cylinderData.cylinderAttributeSet != NULL )			Q3Object_Dispose(cylinderData.cylinderAttributeSet);		Q3Vector3D_Set(&translation, -2, 0, 0);		MyAddTransformedObjectToGroup( myGroup, torusGeo, &translation ) ;		Q3Object_Dispose(torusGeo);		if (torusData.torusAttributeSet != NULL )			Q3Object_Dispose(torusData.torusAttributeSet);	}	return ( myGroup );}