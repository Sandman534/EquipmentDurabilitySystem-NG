import flash.geom.ColorTransform;
import flash.geom.Transform;

class DurabilityWidget {
	private static var DEFAULT_ICON_SOURCE = "skyui/icons_item_psychosteve.swf";

	private var widgetHolder:MovieClip;
	public var scale:Number = 100;
	
	public var iconHolders:Array;
	public var sourceHolders:Array;
	public var textHolders:Array;
	public var valueHolders:Array;
	
	private var iconLoader: MovieClipLoader;
	private var debugTF:TextField;
	private var reverse:Boolean = false;
	private var layout:Number = 0;
	private var positionX:Number = 0;
	private var positionY:Number = 0;
	private var entries:Array;

	// Constructor
	public function DurabilityWidget(swfRoot:MovieClip) {
		super();
		
		widgetHolder = swfRoot;
		// initDebugConsole();
		
		Stage.align = "TL";
		Stage.scaleMode = "noScale";
		
		// Array that holds each widget
		iconHolders = new Array();
		sourceHolders = new Array();
		textHolders = new Array();
		valueHolders = new Array();
		
		// Setup the icon loader
		iconLoader = new MovieClipLoader();
		iconLoader.addListener(this);
		
		entries = new Array();
	}

	// Grow the reusable holder pool only when the visible list needs it.
	private function EnsureHolders(count:Number):Void {
		// Store all the holders into 4 arrays
		for (var i:Number = iconHolders.length; i < count; i++) {
			var mc_icon = org.casalib.util.MovieClipUtil.createEmptyMovieClip(widgetHolder, "icon" + String(i), widgetHolder.getNextHighestDepth(), {_x:0, _y:0});
			iconLoader.loadClip(DEFAULT_ICON_SOURCE, mc_icon);
			mc_icon._visible = true;
			iconHolders.push(mc_icon);
			sourceHolders.push(undefined);
			
			var mc_name = widgetHolder.attachMovie("TextIns", "name" + String(i), widgetHolder.getNextHighestDepth());
			var name_format:TextFormat = mc_name.ins_.getNewTextFormat();
			name_format.font = "$EverywhereMediumFont";
			name_format.size = 20;
			mc_name.ins_.setNewTextFormat(name_format);
			mc_name.ins_.text = "";
			mc_name._visible = true;
			textHolders.push(mc_name);
			
			var mc_value = widgetHolder.attachMovie("TextIns", "value" + String(i), widgetHolder.getNextHighestDepth());
			var value_format:TextFormat = mc_value.ins_.getNewTextFormat();
			value_format.font = "$EverywhereMediumFont";
			value_format.size = 15;
			mc_value.ins_.setNewTextFormat(value_format);
			mc_value.ins_.text = "";
			mc_value._visible = true;
			valueHolders.push(mc_value);
		}
	}
	
	private function onLoadInit(a_icon:MovieClip):Void {
		a_icon.gotoAndStop(1);
	}
	
	public function Setup(a_x:Number, a_y:Number, a_scale:Number, a_layout:Number, a_reverse:Boolean):Void {
		positionX = a_x;
		positionY = a_y;
		scale = a_scale;
		layout = (a_layout == 1) ? 1 : 0;
		reverse = a_reverse;

		LayoutEntries();
	}

	private function LayoutEntries():Void {
		var a_scale:Number = scale;
		// Base Values
		var scaleFactor:Number = a_scale / 100;
		var baseX:Number = Stage.width * positionX;
		var baseY:Number = Stage.height * positionY;
		var spacing:Number = 30 * scaleFactor;
		var textOffset:Number = 15 * scaleFactor;
		var iconXAdjustment:Number = -6 * scaleFactor;
		var iconYAdjustment:Number = 2 * scaleFactor;

		// Horizontal Layout Sensitivities
		var horizontalTextAdjustment:Number = 10 * scaleFactor;
		var horizontalValueAdjustment:Number = 10 * scaleFactor;
		var horizontalCenterAdjustment:Number = 4 * scaleFactor;

		// Vertical Layout Sensitivities
		var verticalTextAdjustment:Number = -2 * scaleFactor;
		var verticalValueAdjustment:Number = 2 * scaleFactor;
		var verticalTextXAdjustment:Number = 4 * scaleFactor;
		var verticalValueXAdjustment:Number = 4 * scaleFactor;

		// Loop through all holders
		for (var i:Number = 0; i < iconHolders.length; i++) {
			var iconX:Number = baseX;
			var iconY:Number = baseY;
			if (layout == 1) {
				iconX += i * spacing;
			} else {
				iconY += i * spacing;
			}

			// Icon X and Y
			iconHolders[i]._x = iconX + iconXAdjustment;
			iconHolders[i]._y = iconY + iconYAdjustment;

			textHolders[i]._xscale = textHolders[i]._yscale = a_scale;
			valueHolders[i]._xscale = valueHolders[i]._yscale = a_scale;

			textHolders[i].ins_.autoSize = "left";
			valueHolders[i].ins_.autoSize = "left";

			if (layout == 1) {
				textHolders[i]._x = iconX + horizontalCenterAdjustment;
				valueHolders[i]._x = iconX + horizontalCenterAdjustment;
				textHolders[i]._y = iconY + (reverse ? textOffset : -textOffset) + horizontalTextAdjustment;
				valueHolders[i]._y = iconY + (reverse ? -textOffset : textOffset) + horizontalValueAdjustment;
				textHolders[i]._rotation = reverse ? 90 : -90;
			} else {
				textHolders[i]._rotation = 0;
				textHolders[i]._y = iconY + verticalTextAdjustment;
				valueHolders[i]._y = iconY + verticalValueAdjustment;
				if (reverse) {
					// Name to the left, health to the right.
					textHolders[i]._x = iconX - textOffset + verticalTextXAdjustment;
					valueHolders[i]._x = iconX + textOffset + verticalValueXAdjustment;
				} else {
					// Name to the right, health to the left.
					textHolders[i]._x = iconX + textOffset + verticalTextXAdjustment;
					valueHolders[i]._x = iconX - textOffset + verticalValueXAdjustment;
				}
			}

			var active:Boolean = i < entries.length;
			iconHolders[i]._visible = active;
			textHolders[i]._visible = active && entries[i].showName;
			valueHolders[i]._visible = active && entries[i].showHealth;
		}
	}
	
	public function UpdateMenu(items:Array):Void {
		if (items == undefined) return;
		entries = items;
		EnsureHolders(entries.length);
		LayoutEntries();

		for (var i:Number = 0; i < entries.length; i++) {
			var entry:Object = entries[i];
			//==============================================================
			// Icon Preparation
			//==============================================================
			var newSource = DEFAULT_ICON_SOURCE;
			var newIcon = entry.icon;
			var newColor = entry.color;
			
			// If we have item Data, otherwise populate with defaults
			if(entry.data) {
				// Call i4 if it is installed
				skse.plugins.InventoryInjector.ProcessEntry(entry.data);
			
				newSource = entry.data.iconSource ? entry.data.iconSource : DEFAULT_ICON_SOURCE;
				newIcon = entry.data.iconLabel ? entry.data.iconLabel : entry.icon;
			}
			
			// Fix the filename
			if (widgetHolder._url.indexOf("exported") != -1 || widgetHolder._url.indexOf("Exported") != -1) {
				newSource = "../" + newSource;
			}
			
			// If our source changed, update
			if(newSource != sourceHolders[i]) {
				sourceHolders[i] = newSource
				iconLoader.loadClip(sourceHolders[i], iconHolders[i]);
			}
			
			// Set Text Color
			var colorTransform = new ColorTransform();
			var transform = new Transform(MovieClip(iconHolders[i]));
			if(typeof(newColor) == "number") {
				colorTransform.rgb = newColor;
			}
			transform.colorTransform = colorTransform;			
			
			// Set the icon and adjust it
			if (newIcon == "none") {
				iconHolders[i].gotoAndStop(1);
			} else {
				iconHolders[i].gotoAndStop(newIcon);
				iconHolders[i]._width = 20 * scale / 100;
				iconHolders[i]._height = 20 * scale / 100;
			}

			//==============================================================
			// Item Names
			//==============================================================				
			textHolders[i].ins_.textColor = entry.color;
			textHolders[i].ins_.text = entry.text;

			if (layout == 1) {
				textHolders[i].ins_._x = 0;
				textHolders[i].ins_._y = -textHolders[i].ins_._height / 2;
			} else if (reverse) {
				// Holder marks the right edge.
				textHolders[i].ins_._x = -textHolders[i].ins_._width;
				textHolders[i].ins_._y = 0;
			} else {
				// Holder marks the left edge.
				textHolders[i].ins_._x = 0;
				textHolders[i].ins_._y = 0;
			}

			//==============================================================
			// Health Value
			//==============================================================			
			if (entry.value == -1) {
				valueHolders[i].ins_.text = "";
			} else {
				valueHolders[i].ins_.textColor = entry.color;
				valueHolders[i].ins_.text = entry.value;
			}

			if (layout == 1) {
				valueHolders[i].ins_._x = -valueHolders[i].ins_._width / 2;
				valueHolders[i].ins_._y = reverse ? -valueHolders[i].ins_._height : 0;
			} else if (reverse) {
				valueHolders[i].ins_._x = 0;
				valueHolders[i].ins_._y = 0;
			} else {
				valueHolders[i].ins_._x = -valueHolders[i].ins_._width;
				valueHolders[i].ins_._y = 0;
			}
		}
	}

	
	private function initDebugConsole():Void {
		var depth:Number = widgetHolder.getNextHighestDepth();

		debugTF = widgetHolder.createTextField("debugTF", depth, 10, 10, 400, 400);
		debugTF.multiline = true;
		debugTF.wordWrap = true;
		debugTF.background = true;
		debugTF.backgroundColor = 0x000000;
		debugTF.textColor = 0xFFFFFF;
		debugTF.border = true;
		
		var fmt:TextFormat = new TextFormat();
		fmt.font = "$EverywhereMediumFont";
		fmt.size = 20;

		debugTF.setNewTextFormat(fmt);
		debugTF.defaultTextFormat = fmt;
		
		debugTF.text = "[Debug Console Ready]\n";
	}
	
	// Append a message to the debug log
	public function DebugLog(msg:String):Void {
		if (!debugTF) return;

		debugTF.text += msg + "\n";
		debugTF.scroll = debugTF.maxscroll; // auto-scroll to bottom
	}
}
