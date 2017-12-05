# luaSTG+ Lua API manual

Translated by [@GCPBunBun](https://twitter.com/gcpbunbun).

## Document Conventions

### Symbols

- **[NewCompatible]**：Indicates that this method adds new features and maintains compatibility with legacy versions.
- **[NewIncompatible]**：Indicates that this method is new and does not exist in legacy versions.
- **[Deprecated]**：Indicates that this method may be removed in subsequent releases and is no longer maintained.
- **[Removed]**：Indicates that the method is removed and is not compatible with legacy versions.
- **[ChangedCompatible]**：Indicates that the method is modified relative to older versions but maintains compatibility.
- **[ChangedIncompatible]**：Indicates that the method has been modified and is incompatible with legacy versions.
- **[Utility]**：Indicates that this method is missing from the benchmark code, does not guarantee full compatibility.

## Translation notes

Paragraphs containing `(?)` contain either incomplete information or need alternate translations.

## Framework Execution Process

- LuaSTGPlus initialisation.
- Initialize Lua engine.
- Load the `launch` file to initialise the configuration, load resource bundles and more.
- Initialize fancy2d engine and window.
- Load the `core.lua` file.
- Execute the `GameInit` global function.
- Start and execute the game loop.
- Destroy the window and exit.

## Encoding

- The program will use **UTF-8** as Lua's encoding. Non-UTF-8 encodings will result in garbled text at run-time. **[ChangedIncompatible]**
- The program will use **UTF-8** as the encoding of resource bundles. If non-UTF-8 encoded characters appear in the resource bundle, the file will not be located. **[ChangedIncompatible]**

## Built-in Variables

- args:table **[ChangedIncompatible]**

	Saves the command line parameters

		Details
			The name of the variable in LuaSTG is arg, which conflicts with the old arguments name arg. As LuaJIT no longer supports the use arg access variable list, in order to reduce coding errors, arg was renamed to lstg.args.
			This may affect the code in launch.

## Built-in Classes

### lstgColor

lstgColor is used to represent a 32-bit colour based on the four a, r, g, b components.

#### Methods

- ARGB(lstgColor):number, number, number, number

	Returns the colour a, r, g, b components.

#### Meta-methods

- __eq(lstgColor, lstgColor):boolean

	Determines whether the two colour values are equal.

- __add(lstgColor, lstgColor):lstgColor

	Adds two colour values with excess component values set to 255.

- __sub(lstgColor, lstgColor):lstgColor **[NewIncompatible]**

	Subtracts the two colour values and sets negative components' values to 0.

- __mul(lstgColor|number, lstgColor|number):lstgColor **[NewCompatible]**

	Multiplies two colors, or multiplies a color by a number, wth excess component values being set to 255.

- __tostring(lstgColor):string

	Returns a string representation of the lstgColor object in the format `lstg.Color(a,r,g,b)` where a, r, g and b are the components' values.

### lstgRand

Based on WELL512's random number generator algorithm, the system clock will be used as the seed. **[ChangedCompatible]**

	Details
		luaSTG's implementation used a linear congruency(?) method to generate random numbers. LuaSTG+ uses WELL512's algorithm instead to generate random numbers (the algorithm excerpt from "Game Programming Gems 7")

	Potential compatibility issues
		The seed (0) initialization generator is used by default in the luaSTG implementation.

#### Methods

- Seed(lstgRand, number)

	Sets a random number seed (Note: number value should not exceed 32bit unsigned integer range).

- GetSeed(lstgRand) **[NewIncompatible]**

	Returns the random number seed.

- Int(lstgRand, min:number, max:number):number

	Generates an integer random number in the [min, max] interval (make sure min <= max).

- Float(lstgRand, min:number, max:number):number

	Produces a floating-point random number in the [min, max] interval (make sure that min <= max).

- Sign(lstgRand):number

	Returns 1 randomly.

#### Meta-methods

- __tostring(lstgRand):string

	Returns the class' name. The value is always `lstg.Rand`

### lstgBentLaserData

This class is used to control the built-in curvilinear laser.

	Details
	Up to 1024 lasers are allowed in luastg+ and up to 512 nodes per laser. Curvy lasers are still somewhat problematic, to be optimised in the future.

#### Methods

- Update(lstgBentLaserData, base\_obj, length, width)

	Passes in the base object and gets the coordinate information of the base object to construct the next laser position. Length and width determines the laser node number and node width. The node width also affects the collision.

- Release(lstgBentLaserData)

	Recycling objects. (No effect in the current implementation, object recycling by GC.)

- Render(lstgBentLaserData, texture:string, blend:string, color:lstgColor, tex\_left:number, tex\_top:number, tex\_weight:number, tex\_height:number, [scale:number=1]) **[NewCompatible]**

	Renders curve laser. The parameters are texture, blend mode, color, and the location of the laser image in the texture. scale is used to control the zooming of the laser in the longitudinal direction.

	Note that the direction of the width of the texture is always used as the direction of the laser beam.

	This function is affected by the global scaling factor.

- CollisionCheck(lstgBentLaserData, x:number, y:number, [rot:number=0, a:number=0, b:number=0, rect:boolean=false]):boolean  **[NewCompatible]**

	Checks if there is a collision between the current object and the object at (x, y).

- BoundCheck(lstgBentLaserData):boolean

	Checks if the current object is within the bounds. If out of bounds, returns false.

## Built-in methods

All built-in methods are grouped in the lstg global table.

### System methods

- SetWindowed(boolean) **[ChangedIncompatible]**

	Sets windowed mode (true) or full-screen mode (false). The default is true.

	**Use only during initialization**，window mode switching is not allowed at run-time.

- SetFPS(number) **[ChangedIncompatible]**

	Sets the FPS lock value. The default is 60 FPS.

	**Use only during initialization**，switching FPS dynamically at run-time is not allowed.

- GetFPS():number

	Gets the current live FPS.

- SetVsync(boolean) **[ChangedIncompatible]**

	Sets whether to use V-Sync. Defaults to true.

	**Use only during initialization**，switching V-Sync modes at run-time is forbidden.

- SetResolution(width:number, height:number)

	Sets the resolution. The default is 640x480.

	For initialization purposes only, it is not allowed to set the resolution dynamically at run time.

- ChangeVideoMode(width:number, height:number, windowed:boolean, vsync:boolean):boolean **[NewIncompatible]**

	Changes video options. Returns true if successful, otherwise returns false and reverts to the previous video mode.

	**Use only at run time.**

- SetSplash(boolean)

	Sets whether to display the cursor. The default is false.

- SetTitle(string)

	Sets the window title. The default is "LuaSTGPlus".

- SystemLog(string)

	Writes a string to the log file.

- Print(...)

	Writes several values ​​to the log.

- LoadPack(path:string, [password:string]) **[NewCompatible]**

	Loads the specified location of the ZIP resource package, optional password.

	Failures will lead to an error.

		Details
			After loading, the resource package has a higher search priority. This means that resource bundles can be loaded by this mechanism to overwrite the files in the underlying resource bundle.
			Once the zip file is opened, it will not be accessible.
			When the file is loaded, the resource package will be searched sequentially according to the priority. If the resource package does not contain the file, the resource package will be loaded from the current directory.

- UnloadPack(path:string)

	Uninstalls the resource package at the specified location, the path name must be consistent.

	If the resource package does not exist, an error will occur.

- ExtractRes(path:string, target:string) **[Utility]**

	Extracts the data in the resource bundle to local.

	Failure will show an error.

- DoFile(path:string)

	Executes the specified path of the script. The executed script will be executed again(?).

	If the file does not exist, the compilation fails, as such, the implementation fails and creates an error.

- ShowSplashWindow([path:string]) **[NewIncompatible]**

	Displays the splash window. The argument specifies the splash image's path.

	If the image fails to load or is empty, a built-in image is used.

### Object pool management methods

- GetnObj():number

	Gets the number of objects in the object pool.

- UpdateObjList() **[Deprecated]**

	Updates the object pool. All objects are sorted and categorized.

	Collation: UID is smaller the more forward it is(?).

		Details
			This function no longer functions in LuaSTG+, and the object tables are always ordered.

- ObjFrame()

	Updates all the objects in the object list and update their properties.

	**It's forbidden to call this method on the co-routine.**

		Details
			Updates these properties in the following order:
				vx += ax
				vy += ay
				x += vx
				y += vy
				rot += omiga
				Updates the bound particle system (if any)

- ObjRender()

	Renders all objects. All objects are sorted at this time.

	**It's forbidden to call this method on the co-routine.**

	Sorting rules: layer first small rendering, if the same layer is in accordance with uid(?).

		Details
			The list of renderings in LuaSTG+ is always ordered and will not be sorted each time.

- SetBound(left:number, right:number, bottom:number, top:number)

	Sets the stage boundaries.

- BoundCheck()

	Performs a border check. Note that BoundCheck only ensures that the center of the object is still within range and does not check the collision box.

	**It's forbidden to call this method on the co-routine.**

- CollisionCheck(A:groupid, B:groupid)

	Group A and B collision detection. If an object in group A collides with an object in group B, the collision callback function for the object in A is executed.

	**It's forbidden to call this method on the co-routine.**

- UpdateXY()

	Refreshes the dx, dy, lastx, lasty, rot (if navi = true) values ​​of the object.

	**It's forbidden to call this method on the co-routine.**

- AfterFrame()

	Refreshes the object timer and ani_timer. If the object is marked as del or kill, the object will be deleted and its resources freed.

	**It's forbidden to call this method on the co-routine.**

		Details
			Objects are only cleaned up after an AfterFrame call. You can still preserve them by updating the object's status field.

- New(class)

	Creates a new object. A new UID will be generated for it.

		Details
			This method uses class to create an object, and after constructing the object, it calls the constructor of class to construct the object.
			The created object has the following properties：
				x, y             coordinates
				dx, dy           (read only) the coordinate increment from the last update
				rot              angle
				omiga            omiga angle increment
				timer            counter
				vx, vy           speed
				ax, ay           acceleration
				layer            rendering priority
				group            collision group
				hide             whether this object should be hidden or rendered
				bound            whether to destroy this object if out of bounds
				navi             whether to automatically update its orientation
				colli            whether to use collision detection
				status           object status; del, kill, or normal
				hscale, vscale   horizontal and vertical scale/zoom level
				class            object's parent class
				a, b             collision box size(?)
				rect             rectangular collision box
				img              (undocumented)
				ani              (read only) animation counter (frame?)
			Index 1 and 2 of the created object are used to store the class and id (immutable!)

			Parent classes must meet the following standard：
				is_class = true
				[1] = initialization function (object, ...)
				[2] = delete function (DEL) (object, ...) **[NewCompatible]**
				[3] = frame function (object)
				[4] = rendering function (object)
				[5] = collision function (object, object)
				[6] = death function (KILL) (object, ...) **[NewCompatible]**
			The above callback functions will be called when the object triggers the corresponding event.

			LuaSTG+ provides up to 32768 spaces for object usage. Exceding this limit will result in an error being reported.

- Del(object, [...]) **[NewCompatible]**

	Notice to delete an object. The flag will be set and the callback function will be called.

	**If you pass multiple parameters in the object, they will be passed to the callback function.**

- Kill(object, [...]) **[NewCompatible]**

	Notice to kill an object. The flag will be set and the callback function will be called.

	**If you pass multiple parameters in the object, will be passed to the callback function.**

- IsValid(object)

	Checks whether the object is valid.

- GetV(object):number, number **[NewIncompatible]**

	Gets the speed of the object. Returns the speed and speed direction.

- SetV(object, v:number, a:number, track:boolean)

	Sets the speed of the object at &lt;Speed ​​Size, Angle&gt;, and set r if track is true(?).

- SetImgState(object, blend:string, a:number, r:number, g:number, b:number)

	Sets the resource status. blend Indicates a mixed mode blending (see SetImageState's documentation below for details.) a, r, g, and b specifies the alpha and color values.

	This function will set the blending mode of the sprite and animation resource bound to the object, and this setting will have effect on all resources of the same name.

- Angle(a:object | x1:number, b:object | y1:number, [x2:number, y2:number]):number

	If a, b are objects then it returns the vector (object b center - object a center) with respect to the positive x-axis angle(?). Otherwise, tan2(y2-y1, x2-x1) is calculated.

- Dist(a:object|number, b:object|number, [c:number, d:number]):number

	If a and b are objects, then it returns the distance between a and b. Otherwise returns the distance between (c, d) and (a, b).

- BoxCheck(object, left:number, right:number, top:number, bottom:number):boolean

	Checks whether the object center is within the given range.

- ResetPool()

	Empties and recycle all objects.

- DefaultRenderFunc(object)

	Invokes the default rendering method on the object.

- NextObject(groupid:number, id:number):number, object **[ChangedIncompatible]**

	Gets the next element in the group. If groupid is an invalid collision group then it returns all objects.

	The first parameter returned is id (idx in luastg), and the second is the object

		Details
			The second parameter accepted in NextObject in luastg is the element index in the group rather than id.
			For efficiency, luastg+ accepts id for the next element and returns the id of the next element(?).

- ObjList(groupid:number):NextObject, number, number **[ChangedIncompatible]**

	Generates a group traversal iterator.

		Details
			Because NextObject behavior changes, ObjList only gets compatibility when used in a for loop.

- ParticleFire(object)

	Launches the particle emitter bound to the object.

- ParticleStop(object)

	Stops binding the particle emitter on the object.

- ParticleGetn(object)

	Returns the number of surviving particles bound to the particle emitter on the object.

- ParticleGetEmission(object)

	Gets the emission density of the particle emitter bound to the object. (A / second)(?)

		Details
			luastg/luastg+ updates, the particle emitter's clock is always 1 / 60s(?)

- ParticleSetEmission(object, count)

	Sets the emission density of the particle emitter bound to the object. (A / second)(?)

### Resource management

luastg / luastg+ provides two resource pools: a global resource pool and level resource pools, for storing resources for different purposes.

The resource pools use string hash tables to manage their resources and its keys can not be repeated.

All load functions load resources into their corresponding pools according to the current resource pool category. Resources are first looked up in level resource pools. If not found, the global resource pool is looked up next.

	Resource types
		1 texture
		2 image
		3 animation
		4 music
		5 sound effects
		6 particles
		7 texture fonts
		8 TTF fonts
		9 shaders **[NewCompatible]**

	Shader support
		Shaders use the D3D9's fx format, which is interconnected with the script system in fx by adding a comment called "binding" to the annotation(?)
		Allow the end of the assignment type lua end there(?)
			string：interpreted and positioned to the texture resource
			number：interpreted as float
			lstgColor：interpreted as float4
		Shader is currently used only by the PostEffect function(s).

	Render target support
		RenderTarget's size will be consistent with the screen/window's size and can not be customized.
		Notice that the RenderTarget can either be rendered as a render or as an input source to a pixel, so at the same time any renderer on it is invalid while the RenderTarget is being used. For efficiency, lstg + did not check this behavior, the use must be avoided, so as to avoid causing graphics driver crash and other issues(?)

- RemoveResource(pool:string, [type:integer, name:string]) **[NewCompatible]**

	If there is only one parameter, then delete all the resources in a pool. Otherwise delete a resource from the corresponding pool. Optional global parameter or stage(?).

	If the resource is still in use, it will remain loaded until the relevant object is released.

- CheckRes(type:integer, name:string):string|nil

	Gets a category of resources, usually used to detect the existence of resources(?).

		Details
			The method will be based on the name of the global resource pool to find, if it returns global(?)
			If there is no resource found in the global resource table, then find in the level resource pool, if there is return stage.
		  If there is no resource, returns nil.

- EnumRes(type:integer):table, table

	Enumerates some type of resource in the resource pool and return the names of all resources of this type in the global resource pool and the level resource pool in turn.

- GetTextureSize(name:string):number, number

	Gets the width and height of a texture.

- LoadTexture(name:string, path:string, [mipmap:boolean=false])

	Loads texture, support a variety of formats but devaluation(?) PNG. Which mipmap texture chain(?).

- LoadImage(name:string, tex\_name:string, x:number, y:number, w:number, h:number, [a:number, [b:number, [rect:boolean]]])

	Creates an image using a texture. x, y specifies the coordinates of the upper left corner of the texture (the top left corner of the texture is (0,0), the bottom right is the positive direction), w, h specifies the size of the image, a, b, rect specifies the horizontal and vertical collisions detection and determines the shape.

		Details
			When an img field is assigned to an object, its a, b, and rect properties are automatically assigned to the object.

- SetImageState(name:string, blend\_mode:string, \[vertex\_color1:lstgColor, vertex\_color2:lstgColor, vertex\_color3:lstgColor, vertex\_color4:lstgColor\])

	Sets the image state, an optional color parameter is used to set all the vertices or given all the vertices in 4 colors.

		The blending parameter is optional and can be an empty string
			""          The default: mul+alpha
			"mul+add"   The "mul + add" vertex color uses multiplication and the target blend uses addition
			"mul+alpha" (The default.) Vertex colors use multiplication and target blending uses alpha blending
			"mul+sub"   Vertex uses multiplication, the result = the color on the image - the color on the screen(?) **[NewIncrease]**
			"mul+rev"   Vertex color using multiplication, result = color on screen - color on image(?) **[NewIncrease]**
			"add+add"   Vertex color and destination both use additive blending
			"add+alpha" Vertex color uses addition and the destination blend uses alpha blending
			"add+sub"   Vertex color uses addition blending, result = color on image - color on screen(?) **[NewIncrease]**
			"add+rev"   Vertex color uses additive blending, result = screen color - color on image(?) **[NewIncrease]**

- SetImageCenter(name:string, x:number, y:number)

	Sets the image center, with x and y being relative to the upper left corner of the image.

- LoadAnimation(name:string, tex\_name:string, x:number, y:number, w:number, h:number, n:integer, m:integer, intv:integer, [a:number, [b:number, [rect:boolean]]])

	Loading animation. a, b, rect with the same meaning. Where x, y specifies the upper left corner of the first frame position, w, h specifies the size of a frame. n and m specify the number of vertical and horizontal division, in order of priority. intv specifies the frame interval.

	The animation is continuosly re-played.

- SetAnimationState(name:string, blend\_mode:string, \[vertex\_color1:lstgColor, vertex\_color2:lstgColor, vertex\_color3:lstgColor, vertex\_color4:lstgColor\])

	Similar to SetImageState.

- SetAnimationCenter(name:string, x:number, y:number)

	Similar to SetImageCenter.

- LoadPS(name:string, def\_file:string, img_name:string, [a:number, [b:number, [rect:boolean]]])

	Load particle system. def\_file is the definition file, img\_name is the particle image. a, b, rect have the same meaning as above.

	Particle file structure used by HGE.

- LoadFont(name:string, def\_file:string, \[bind\_tex:string, mipmap:boolean=true\])

	For loading texture fonts. name indicates the name, def\_file indicates the definition file, mipmap indicates whether or not to create a texture chain, created by default. The bind\_tex parameter is used by the f2d texture font to indicate the full path of the bound texture.

		Details
			LuaSTG+ supports HGE texture fonts and fancy2d texture fonts (xml format).
			For HGE fonts, the texture files are looked for under the same-level directory as the definition files, and for f2d fonts, the bind\_tex parameter is used to find the texture.

- SetFontState(name:string, blend_mode:string, [color:lstgColor])

	Sets the font color, mixed mode(?). See the concrete mixing options above(?)

- SetFontState2(...)  **[Removed]**

	This method was used to set the texture font of HGE, LuaSTG+ does not support this method anymore.

		Details
			Most of the existing code does not use this method, you can ignore it.

- LoadTTF(name:string, path:string, width:number, height:number) **[ChangedIncompatible]**

	This method is used to load TTF fonts. name specifies the resource name, path specifies the loading path, width and height specify the font size, it is recommended to set the same value.

		Details
			The LoadTTF method is vastly different than in LuaSTG. Due to the built-in font rendering engine, the font does not need to be extracted and imported into the system under the current implementation.
			But the opposite, you can not use such as bold, tilt and other effects. At the same time the parameters are reduced to four.
			In addition, if you can not load the font file in the path location(?).

- RegTTF(...) **[Deprecated]**

	This function no longer works. Will be removed in a future version.

- LoadSound(name:string, path:string)

	Loads sound effects. Only supports wav or ogg, it is recommended to use wav format.

		Details
			Sound will be loaded into memory. Do not use longer audio files for sound effects.
			For wav format, due to the current implementation, it does not support non-standard, compressed format. (Such as AU export with metadata will lead to errors.)

- LoadMusic(name:string, path:string, end:number, loop:number)

	Loads music. name specifies the name, path specifies the path, end and loop specify the end of the link(?) and the duration (in seconds), that is, the link range is end-loop ~ end.

	Only supports wav or ogg, and it's recommended to use ogg format.

		Details
			Music will be streamed into memory and will not be completely decoded into memory at once. It's not recommended to use wav format, please use ogg as music format.
			You can set the loop of music by describing the loop section. When the music is played to end end will converge to start. This step in the decoder to ensure the perfect convergence.

- LoadFX(name:string, path:string) **[NewIncompatible]**

	Loads Shader effects.

- CreateRenderTarget(name:string) **[NewIncompatible]**

	Creating a RenderTarget named name will be placed in the Texture pool, which means it can be used like a texture.

- IsRenderTarget(name:string) **[NewIncompatible]**

	Checks if a texture is a RenderTarget.

----------

### Rendering methods

LuaSTG / LuaSTG+ uses the Cartesian coordinate system (right up) as the window coordinate system, and with the lower left corner of the screen as the origin. Viewport and mouse messages will use that as a reference.

There is a global image scaling factor in LuaSTG+ for rendering in different modes that will affect the object's rendering size, the bounding size of the image to be bound, and the partial rendering function.

LuaSTG / LuaSTG+ does not turn Z-Buffer for deep culling and does this manually by sorting.

In addition, starting with LuaSTG+, rendering and updating will be done independently in both functions. All rendering operations must be performed in RenderFunc.

**Special Note for RenderTargets: this RenderTarget can no longer be rendered when the RenderTarget is in the screen buffer(?).**

- BeginScene()

	Notification render begins. This method must be called in RenderFunc. All rendering actions must be done between BeginScene / EndScene.

		Incompatibility issues
			Starting from LuaSTG+, all rendering must be moved to RenderFunc.

- EndScene()

	Notifies the end of the rendering. This method must be called in RenderFunc.

- RenderClear(lstgColor)

	Empties the screen with the specified color. Clears the depth buffer while clearing the color.

- SetViewport(left:number, right:number, bottom:number, top:number)

	Setting the viewport will affect cropping and rendering.

- SetOrtho(left:number, right:number, bottom:number, top:number)

	Sets the orthographic projection matrix. left represents the minimum value of the x-axis, right represents the maximum value of the x-axis, bottom represents the minimum value of the y-axis, and top represents the maximum value of the y-axis.

		Details
			The orthographic projection matrix created will limit the z-axis within the [0,1] interval.

- SetPerspective(eyeX:number, eyeY:number, eyeZ:number, atX:number, atY:number, atZ:number, upX:number, upY:number, upZ:number, fovy:number, aspect:number, zn:number, zf:number)

	Sets the perspective projection matrix and observation matrix. (eyeX, eyeY, eyeZ) indicates the observer's position, (atX, atY, atZ) indicates the observation target, and (upX, upY, upZ) indicates the viewer's upward direction. fovy describes the viewing angle range (in radians), aspect describes the aspect ratio, zn and zf describes the z-axis clipping distance.

- Render(image_name:string, x:number, y:number, [rot:number=0, [hscale:number=1, [vscale:number=1, [z:number=0.5]]]])

	Renders an image. (x, y) specifies the center point, rot specifies the rotation (in radians), (hscale, vscale) specifies XY-axis scaling, and z specifies the Z coordinate

	If you specify hscale without specifying vscale then vscale = hscale.

	This function is affected by the global image scaling factor.

- RenderRect(image_name:string, left:number, right:number, bottom:number, top:number)

	Renders the image in a matrix range. Z = 0.5.

- Render4V(image_name:string, x1:number, y1:number, z1:number, x2:number, y2:number, z2:number, x3:number, y3:number, z3:number, x4:number, y4:number, z4:number)

	Gives four vertex rendered images. Z = 0.5(?)

- SetFog([near:number, far:number, [color:lstgColor = 0x00FFFFFF]])

	If the parameters are empty, the fog effect will be turned off. Otherwise sets a fog from near to far.

- RenderText(name:string, text:string, x:number, y:number, [scale:number=1, align:integer=5])

	Uses texture fonts to render a piece of text. The parameter name specifies the texture name, text specifies the string, x, y specify the coordinates, align specified alignment mode.

	This function is affected by the global image scaling factor.

		Details
			Alignment mode specify the rendering center and alignment direction
				Top left					0 + 0  0
				Center left				0 + 4  4
				Bottom left				0 + 8  8
				Vertical top			1 + 0  1
				Vertical middle		1 + 4  5
				Vertical bottom		1 + 8  9
				Top right					2 + 0  2
				Center right			2 + 4  6
				Lower right				2 + 8  10
			Due to the new layout mechanism, there will be a slight lateral error when rendering HGE fonts. Please adjust them manually.

- RenderTexture(tex\_name:string, blend:string, vertex1:table, vertex2:table, vertex3:table, vertex4:table)

	Renders the texture directly.

		Details
			vertex1 ~ 4 specify the coordinates of each vertex, which must contain the following items:
			[1] = X coordinate
		  [2] = Y coordinate
		  [3] = Z coordinate
		  [4] = U coordinates (based on texture size)
		  [5] = V coordinate (based on texture size)
		  [6] = vertex color
			Note that the function is less efficient. Consider using a cached vertices table.

- RenderTTF(name:string, text:string, left:number, right:number, bottom:number, top:number, fmt:integer, blend:lstgColor)  **[ChangedIncompatible]**

	Renders TTF fonts.

	This function is affected by the global image scaling factor.

		Details
		Temporarily does not support rendering formatting.
		Interface has been unified into the screen coordinate system, does not need to be converted in the code(?).

- PushRenderTarget(name:string) **[NewIncompatible]**

	Puts a RenderTarget as a screen buffer and push it onto the stack.

	This is an advanced method.

		Details
			LuaSTG+ uses the stack to manage RenderTargets, which means nested RenderTargets can be used.

- PopRenderTarget()  **[NewIncompatible]**

	Removes the currently used RenderTarget from the stack.

	This is an advanced method.

- PostEffect(name:string, fx:string, blend:string, [args:table]) **[NewIncompatible]**

	Applies PostEffect. Parameter specifying the parameter table passed to the FX will affect the subsequent use of the FX(?).

	hich blend specified in posteffect is to be drawn in what forms the screen, then the first component of blend will be invalid(?).

	This is an advanced method.

		Details
			Only PostEffect will render all pass in the first technique.
			The following semantic annotations (case insensitive) can be used to capture objects in PostEffect:
				POSTEFFECTTEXTURE Gets the capture texture (texture2d type) of the posteffect.
				VIEWPORT Gets the viewport size (vector type).
				SCREENSIZE get the screen size (vector type).

- PostEffectCapture() **[NewIncompatible]**

	Starts capturing the drawing data.

	From this step on, all subsequent render operations will take place in the PostEffect buffer.

	This operation is equivalent to `PushRenderTarget(InternalPostEffectBuffer)`.

	This is an advanced method.

- PostEffectApply(fx_name:string, blend:string, [args:table]) **[NewIncompatible]**

	Ends the screen capture and apply PostEffect.

	This operation is equivalent to:

	```lua
	PopRenderTarget(InternalPostEffectBuffer)
	PostEffect(InternalPostEffectBuffer, fx_name, blend, args)
	```

	Due to the need to pair `InternalPostEffectBuffer`, the RenderTarget stack must be `InternalPostEffectBuffer`.

	In other words, the code must satisfy:

	```lua
	PostEffectCapture(...)
	...  -- Paired Push / PopRenderTarget function calls
	PostEffectApply(...)
	```

	This is an advanced method.

### Audio playback methods

- PlaySound(name:string, vol:number, [pan:number=0.0])

	Plays a sound effect. vol is the volume, in the range of [0~1]，pan is the balance(?), and the value [-1~1]。

		Details
			LuaSTG+ plays only one sound at a time, and if one sound (of the same resource name?) is already playing it will interrupt the playback and start from scratch.

- StopSound(name:string) **[NewIncompatible]**

	Stops playing audio. name is the name of the resource.

- PauseSound(name:string) **[NewIncompatible]**

	Pauses playing audio. name is the name of the resource.

- ResumeSound(name:string) **[NewIncompatible]**

	Continues playing audio. name is the name of the resource.

- GetSoundState(name:string):string **[NewIncompatible]**

	Gets sound playback status. Will return paused, playing, or stopped.

- PlayMusic(name:string, [vol:number=1.0, position:number=0])

	Plays music. name is the resource name, vol is the volume, and position is the starting playback position (in seconds.)

- StopMusic(name:string)

	Stops playing music. This will return the music playback position to the beginning. name is the name of the resource.

- PauseMusic(name:string)

	Pauses playing music. name is the name of the resource.

- ResumeMusic(name:string)

	Resumes music playback. name is the name of the resource.

- GetMusicState(name:string):string

	Gets music playback status. Returns paused, playing, or stopped.

- UpdateSound()  **[Deprecated]**

	**The method has no effect, will be removed in subsequent versions.**

- SetSEVolume(vol:number)

	Sets the global sound volume, will affect the volume of subsequent sound effects.

	The volume value range is [0,1].

- SetBGMVolume([vol:number] | [name:string, vol:number]) **[NewCompatible]**

	If the number of parameters is 1, set the global music volume. This will affect the volume of subsequent music playback.

	If the number of parameters is 2, set the playback volume of the specified music.

	The volume value range is [0,1].

### Input methods

Currently, handle inputs are mapped to locations 0x920xB1 and 0xDF0xFE (2 handles total, 32 keys).

Amongst them, the X-axis Y-axis position is mapped to the first four buttons, corresponding to up, down, left and right.

- GetKeyState(vk\_code:integer):boolean

	Gives virtual key code detection when pressed.

		Details
			VK_CODE specific meaning, please refer to MSDN's documentation.

- GetLastKey():integer

	Returns the virtual key code for the last key entered.

- GetLastChar():string

	Returns the last character entered.

- GetMousePosition():number,number **[NewIncompatible]**

	Gets the location of the mouse relative to the bottom left corner of the window.

- GetMouseState(button:integer):boolean **[NewIncompatible]**

	Checks if the mouse button is pressed. Button may be 0, 1 or 2, corresponding to the left, middle, and right button, respectively.

----------

### 杂项

- Snapshot(file\_path:string)

	Saves a screenshot to file file\_path. Uses PNG format.

- Execute(path:string, [arguments:string=nil, directory:string=nil, wait:boolean=true]):boolean  **[NewIncompatible]**

	Executes external program. Parameter path is for the executable program path, arguments for the parameters, directory for the working directory, wait indicates whether to wait for the program execution is completed.

	Returns true if successful, false if failed.

### Built-in mathematical methods

The following mathematical functions directly call the C standard library functions' equivalents (resulting in better performance than Lua's math module? To be confirmed. It's LuaJIT after all!)

- sin(ang)
- cos(ang)
- asin(v)
- acos(v)
- tan(ang)
- atan(v)
- atan2(y,x)

### Built-in object constructor methods

- Rand()

	Constructs a random number generator. The current system time (ticks) is used as the seed.

- Color(\[argb:integer\] | [a:integer, r:integer, g:integer, b:integer])

	Constructs a Color.

- BentLaserData()

	Constructs a curved laser controller(?)

### Debug methods

- ObjTable():table

	This method returns the object pool table. Use with caution!

- Registry():table  **[Removed]**

	Returns to the registry.

		Details
			This method has been removed because it's insecure or inaccurate.

## Global callback functions

The following callback function must be defined in the global scope of the script and are called automatically by LuaSTG+.

- GameInit()

	After the game frame is initialized, the game loop is called before it starts.

- FocusLoseFunc()

	Called when the render window loses focus.

- FocusGainFunc()

	Called when the render window gets the focus.

- FrameFunc():boolean

	Frame processing function. It's called each frame, meant to be used to handle your game's logic.

	It must return a boolean value. If true, it'll terminate the game loop, thus quitting the game. Return false (to be confirmed: or don't return anything at all? This doesn't seem to crash the game) to keep the game and the main loop running.

- RenderFunc() **[NewIncompatible]**

	Used to render the scene after each frame (FrameFunc) is called.

## Third party libraries

### cjson **[NewIncompatible]**

#### Methods

For more information, please refer to [cjson](http://www.kyne.com.au/~mark/software/lua-cjson.php)'s' home page.

- encode(table):string

	Encodes a Lua table as a JSON string.

- decode(string):table

	Decodes a JSON string to a Lua table.
