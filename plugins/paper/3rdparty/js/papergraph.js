/*!

The MIT License (MIT)

Copyright (c) 2015 Rolf Fleischmann (@w00dn) http://vol-2.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

function Pg(paper){

    var pg = this

    pg.styleBar = null;

    pg.color = function(){
        var toCSS = function(c){
            if ( c.a === 1 ){
                return c.toString()
            } else {
                var argb = c.toString()
                var rgba = "#" + argb.substr(3) + argb.substr(1, 2)
                return rgba
            }
        }

        var fromPaper = function(c){
            return Qt.rgba(c.red, c.green, c.blue, c.alpha)
        }

        return {
            toCSS: toCSS,
            fromPaper: fromPaper
        }
    }()

    pg.styles = function(){

        var update = function(opt){
            if ( opt.foreground )
                applyFillColorToSelection(pg.color.toCSS(opt.foreground))
            if ( opt.background )
                applyStrokeColorToSelection(pg.color.toCSS(opt.background))
            if ( opt.opacity )
                applyOpacityToSelection(opt.opacity)
            if ( opt.blendMode )
                applyBlendModeToSelection(opt.blendMode)
            if ( opt.strokeSize )
                applyStrokeWidthToSelection(opt.strokeSize)
        }

        var applyFillColorToSelection = function(colorString) {
            var items = pg.selection.getSelectedItems();
            for(var i=0; i<items.length; i++) {
                var item = items[i];
                if(pg.item.isPGTextItem(item)) {
                    for(var j=0; j<item.children.length; j++) {
                        var child = item.children[j];
                        for(var k=0; k<child.children.length; k++) {
                            var path = child.children[k];
                            if(!path.data.isPGGlyphRect) {
                                path.fillColor = colorString;
                            }
                        }
                    }
                } else {
                    if(pg.item.isPointTextItem(item) && !colorString) {
                        colorString = 'rgba(0,0,0,0)';
                    }
                    item.fillColor = colorString;
                }
            }
            pg.undo.snapshot('applyFillColorToSelection');
        };

        var applyStrokeColorToSelection = function(colorString) {
            var items = pg.selection.getSelectedItems();

            for(var i=0; i<items.length; i++) {
                var item = items[i];
                if(pg.item.isPGTextItem(item)) {
                    for(var j=0; j<item.children.length; j++) {
                        var child = item.children[j];
                        for(var k=0; k<child.children.length; k++) {
                            var path = child.children[k];
                            if(!path.data.isPGGlyphRect) {
                                path.strokeColor = colorString;
                            }
                        }
                    }
                } else {
                    item.strokeColor = colorString;
                }
            }
            pg.undo.snapshot('applyStrokeColorToSelection');
        };

        var applyOpacityToSelection = function(alpha) {
            var items = pg.selection.getSelectedItems();

            for(var i=0; i<items.length; i++) {
                items[i].opacity = alpha;
            }
            pg.undo.snapshot('setOpacity');
        };


        var applyBlendModeToSelection = function(mode) {
            var items = pg.selection.getSelectedItems();

            for(var i=0; i<items.length; i++) {
                items[i].blendMode = mode;
            }
            pg.undo.snapshot('setBlendMode');
        };


        var applyStrokeWidthToSelection = function(value) {
            var items = pg.selection.getSelectedItems();

            for(var i=0; i<items.length; i++) {
                var item = items[i];

                if(pg.group.isGroup(item)) {
                    for(var j=0; j<item.children.length; j++) {
                        var child = item.children[j];
                        child.strokeWidth = value;
                    }
                    continue;

                } else {
                    item.strokeWidth = value;
                }
            }
            pg.undo.snapshot('setStrokeWidth');
        };


        var applyActiveStyle = function(item){
            if(pg.group.isGroup(item)) {
                item.opacity = pg.styleBar.opacity;
                item.blendMode = pg.styleBar.blendMode;
            } if(pg.item.isPGTextItem(item)) {
                for(var j=0; j<item.children.length; j++) {
                    var child = item.children[j];
                    for(var k=0; k<child.children.length; k++) {
                        var path = child.children[k];
                        if(!path.data.isPGGlyphRect) {
                            path.fillColor = pg.color.toCSS(pg.styleBar.foreground);
                        }
                    }
                }
            } else {
                item.fillColor = pg.color.toCSS(pg.styleBar.foreground);
                item.strokeColor = pg.color.toCSS(pg.styleBar.background);
                item.opacity = pg.styleBar.opacity;
                item.strokeWidth = pg.styleBar.strokeSize;
                item.blendMode = pg.styleBar.blendMode;
            }
            return item;
        }

        return {
            applyActiveStyle : applyActiveStyle,
            update: update
        }
    }();

    pg.guides = function() {

        var guideBlue = '#009dec';
        var guideGrey = '#aaaaaa';

        var hoverItem = function(hitResult) {
            var segments = hitResult.item.segments;
            var clone = new paper.Path(segments);
            setDefaultGuideStyle(clone);
            if(hitResult.item.closed) {
                clone.closed = true;
            }
            clone.parent = pg.layer.getGuideLayer();
            clone.strokeColor = guideBlue;
            clone.fillColor = null;
            clone.data.isHelperItem = true;
            clone.bringToFront();

            return clone;
        };


        var hoverBounds = function(item) {
            var rect = new paper.Path.Rectangle(item.internalBounds);
            rect.matrix = item.matrix;
            setDefaultGuideStyle(rect);
            rect.parent = pg.layer.getGuideLayer();
            rect.strokeColor = guideBlue;
            rect.fillColor = null;
            rect.data.isHelperItem = true;
            rect.bringToFront();

            return rect;
        };


        var rectSelect = function(event, color) {
            var half = new paper.Point(0.5 / paper.view.zoom, 0.5 / paper.view.zoom);
            var start = event.downPoint.add(half);
            var end = event.point.add(half);
            var rect = new paper.Path.Rectangle(start, end);
            var zoom = 1.0/paper.view.zoom;
            setDefaultGuideStyle(rect);
            if(!color) color = guideGrey;
            rect.parent = pg.layer.getGuideLayer();
            rect.strokeColor = color;
            rect.data.isRectSelect = true;
            rect.data.isHelperItem = true;
            rect.dashArray = [3.0*zoom, 3.0*zoom];
            return rect;
        };


        var line = function(from, to, color) {
            var line = new paper.Path.Line(from, to);
            var zoom = 1/paper.view.zoom;
            setDefaultGuideStyle(line);
            if (!color) color = guideGrey;
            line.parent = pg.layer.getGuideLayer();
            line.strokeColor = color;
            line.strokeColor = color;
            line.dashArray = [5*zoom, 5*zoom];
            line.data.isHelperItem = true;
            return line;
        };


        var crossPivot = function(center, color) {
            var zoom = 1/paper.view.zoom;
            var star = new paper.Path.Star(center, 4, 4*zoom, 0.5*zoom);
            setDefaultGuideStyle(star);
            if(!color) color = guideBlue;
            star.parent = pg.layer.getGuideLayer();
            star.fillColor = color;
            star.strokeColor = color;
            star.strokeWidth = 0.5*zoom;
            star.data.isHelperItem = true;
            star.rotate(45);

            return star;
        };


        var rotPivot = function(center, color) {
            var zoom = 1/paper.view.zoom;
            var path = new paper.Path.Circle(center, 3*zoom);
            setDefaultGuideStyle(path);
            if(!color) color = guideBlue;
            path.parent = pg.layer.getGuideLayer();
            path.fillColor = color;
            path.data.isHelperItem = true;

            return path;
        };


        var label = function(pos, content, color) {
            var text = new paper.PointText(pos);
            if(!color) color = guideGrey;
            text.parent = pg.layer.getGuideLayer();
            text.fillColor = color;
            text.content = content;
        };


        var setDefaultGuideStyle = function(item) {
            item.strokeWidth = 1/paper.view.zoom;
            item.opacity = 1;
            item.blendMode = 'normal';
            item.guide = true;
        };


        var getGuideColor = function(colorName) {
            if(colorName === 'blue') {
                return guideBlue;
            } else if(colorName === 'grey') {
                return guideGrey;
            }
        };


        var getAllGuides = function() {
            var allItems = [];
            for(var i=0; i<paper.project.layers.length; i++) {
                var layer = paper.project.layers[i];
                for(var j=0; j<layer.children.length; j++) {
                    var child = layer.children[j];
                    // only give guides
                    if(!child.guide) {
                        continue;
                    }
                    allItems.push(child);
                }
            }
            return allItems;
        };


        var getExportRectGuide = function() {
            var guides = getAllGuides();
            for(var i=0; i<guides.length; i++){
                if(guides[i].data && guides[i].data.isExportRect) {
                    return guides[i];
                }
            }
        };


        var removeHelperItems = function() {
            pg.helper.removePaperItemsByDataTags(['isHelperItem']);
        };


        var removeAllGuides = function() {
            pg.helper.removePaperItemsByTags(['guide']);
        };


        var removeExportRectGuide = function() {
            pg.helper.removePaperItemsByDataTags(['isExportRect']);
        };


        return {
            hoverItem: hoverItem,
            hoverBounds: hoverBounds,
            rectSelect: rectSelect,
            line: line,
            crossPivot: crossPivot,
            rotPivot: rotPivot,
            label: label,
            removeAllGuides: removeAllGuides,
            removeHelperItems: removeHelperItems,
            removeExportRectGuide: removeExportRectGuide,
            getAllGuides: getAllGuides,
            getExportRectGuide: getExportRectGuide,
            getGuideColor: getGuideColor,
            setDefaultGuideStyle:setDefaultGuideStyle
        };

    }();


    pg.hover = function() {

        var hoveredItem;
        var handleHoveredItem = function(hitOptions, event) {
//            gc() //win
            var hitResult = paper.project.hitTest(event.point, hitOptions);
//            gc() //win
            if(hitResult) {
                if((hitResult.item.data && hitResult.item.data.noHover)) {
                    return;
                }
                if(hitResult !== hoveredItem) {
                    clearHoveredItem();
                }
                if(	hoveredItem === undefined && hitResult.item.selected === false) {

                    if(pg.item.isBoundsItem(hitResult.item)) {
                        hoveredItem = pg.guides.hoverBounds(hitResult.item);

                    } else if(pg.group.isGroupChild(hitResult.item)) {
                        hoveredItem = pg.guides.hoverBounds(pg.item.getRootItem(hitResult.item));

                    } else {
                        hoveredItem = pg.guides.hoverItem(hitResult);
                    }
                }

            } else {
                clearHoveredItem();
            }
        };


        var clearHoveredItem = function() {
            if(hoveredItem !== undefined) {
                hoveredItem.remove();
                hoveredItem = undefined;
            }
            paper.view.update();
        };


        return {
            handleHoveredItem: handleHoveredItem,
            clearHoveredItem: clearHoveredItem
        };
    }();



    pg.item = function() {

        var isBoundsItem = function(item) {
            if( item.className === 'PointText' ||
                item.className === 'Shape' ||
                item.className === 'PlacedSymbol' ||
                item.className === 'Raster') {
                return true;

            } else {
                return false;
            }
        };


        var isPathItem = function(item) {
            return item.className === 'Path';
        };


        var isCompoundPathItem = function(item) {
            return item.className === 'CompoundPath';
        };


        var isGroupItem = function(item) {
            return item && item.className && item.className === 'Group';
        };


        var isPointTextItem = function(item) {
            return item.className === 'PointText';
        };


        var isLayer = function(item) {
            return item.className === 'Layer';
        };


        var isPGTextItem = function(item) {
            return getRootItem(item).data.isPGTextItem;
        };


        var setPivot = function(item, point) {
            if(isBoundsItem(item)) {
                item.pivot = item.globalToLocal(point);
            } else {
                item.pivot = point;
            }
        };


        var getPositionInView = function(item) {
            var itemPos = new paper.Point();
            itemPos.x = item.position.x - paper.view.bounds.x;
            itemPos.y = item.position.y - paper.view.bounds.y;
            return itemPos;
        };


        var setPositionInView = function(item, pos) {
            item.position.x = paper.view.bounds.x + pos.x;
            item.position.y = paper.view.bounds.y + pos.y;
        };

        var getRootItem = function(item) {
            if(item.parent.className === 'Layer') {
                return item;
            } else {
                return getRootItem(item.parent);
            }
        };


        return {
            isBoundsItem: isBoundsItem,
            isPathItem: isPathItem,
            isCompoundPathItem: isCompoundPathItem,
            isGroupItem: isGroupItem,
            isPointTextItem: isPointTextItem,
            isLayer: isLayer,
            isPGTextItem: isPGTextItem,
            setPivot: setPivot,
            getPositionInView: getPositionInView,
            setPositionInView: setPositionInView,
            getRootItem: getRootItem
        };
    }();

    // function related to groups and grouping

    pg.group = function() {

        var groupSelection = function() {
            var items = pg.selection.getSelectedItems();
                if(items.length > 0) {
                var group = new paper.Group(items);
                pg.selection.clearSelection();
                pg.selection.setItemSelection(group, true);
                pg.undo.snapshot('groupSelection');
                jQuery(document).trigger('Grouped');
                return group;
            } else {
                return false;
            }
        };


        var ungroupSelection = function() {
            var items = pg.selection.getSelectedItems();
            ungroupItems(items);
            pg.statusbar.update();
        };


        var groupItems = function(items) {
            if(items.length > 0) {
                var group = new paper.Group(items);
                jQuery(document).trigger('Grouped');
                pg.undo.snapshot('groupItems');
                return group;
            } else {
                return false;
            }
        };


        // ungroup items (only top hierarchy)
        var ungroupItems = function(items) {
            var emptyGroups = [];
            for(var i=0; i<items.length; i++) {
                var item = items[i];
                if(isGroup(item) && !item.data.isPGTextItem) {
                    ungroupLoop(item, false);

                    if(!item.hasChildren()) {
                        emptyGroups.push(item);
                    }
                }
            }

            // remove all empty groups after ungrouping
            for(var j=0; j<emptyGroups.length; j++) {
                emptyGroups[j].remove();
            }
            jQuery(document).trigger('Ungrouped');
            pg.undo.snapshot('ungroupItems');
        };


        var ungroupLoop = function(group, recursive) {
            // don't ungroup items that are no groups
            if(!group || !group.children || !isGroup(group)) return;

            group.applyMatrix = true;
            // iterate over group children recursively
            for(var i=0; i<group.children.length; i++) {
                var groupChild = group.children[i];
                if(groupChild.hasChildren()) {

                    // recursion (groups can contain groups, ie. from SVG import)
                    if(recursive) {
                        ungroupLoop(groupChild, true);
                    } else {
                        groupChild.applyMatrix = true;
                        group.layer.addChild(groupChild);
                        i--;
                    }

                } else {
                    groupChild.applyMatrix = true;
                    // move items from the group to the activeLayer (ungrouping)
                    group.layer.addChild(groupChild);
                    i--;
                }
            }
        };


        var getItemsGroup = function(item) {
            var itemParent = item.parent;

            if(isGroup(itemParent)) {
                return itemParent;
            } else {
                return null;
            }
        };


        var isGroup = function(item) {
            return pg.item.isGroupItem(item);
        };


        var isGroupChild = function(item) {
            var rootItem = pg.item.getRootItem(item);
            return isGroup(rootItem);
        };


        return {
            groupSelection: groupSelection,
            ungroupSelection: ungroupSelection,
            groupItems: groupItems,
            ungroupItems: ungroupItems,
            getItemsGroup: getItemsGroup,
            isGroup: isGroup,
            isGroupChild:isGroupChild
        };

    }();

    pg.layerPanel = function(){
        return {
            onLayersChanged: null
        }
    }();

    pg.layer = function() {

        var layerNames = ['coasts','sisters','buttons','spaces','teeth','arguments','clubs','thrills','vegetables','sausages','locks','kicks','insects','cars','trays','clams','legs','humor','levels','jelly','competition','cubs','quivers','flags','pins','floors','suits','actors','queens','appliances','dogs','plates','donkeys','coughing','tops','covers','dads','breath','sacks','thumbs','impulse','linens','industry','cobwebs','babies','volcanoes','beef','values','reason','birds','rays','stages','wrenches','uncles','water','bits','knees','jails','jellyfish','treatment','scissors','cars','vacation','lips','ovens','language','money','soup','knowledge','eggs','sponges','basins','coats','chalk','scarfs','letters','rooms','horses','touch','carpentry','honey','effects','flight','debt','boards','advice','brakes','fish','camps','the north','trains','balance','wounds','routes','guitars','receipts','cracks','sex','chance','looks','windows','girls','partners','stars','yam','smashing','existence','keys','flowers','talk','sons','wood','fuel','cakes','wealth','sofas','homes','desks','screws','bells','ears','juice','dogs','force','crooks','attraction','knots','lumber','activity','moons','creators','apparel','iron','crayons','tanks','twigs','condition','songs','snails','driving','cheese','rails','rings','shows','vans','love','moms','schools','pets','dust','experience','cellars','questions','rolls','power','scale','connection','grades','magic','maids','ships','leather','exchange','pigs','sticks','rhythm','distribution','harmony','dinosaurs','towns','rings','cribs','toes','heat','buckets','cables','books','drinks','grass','aunts','turkey','laborer','oil','discussion','drawers','oceans','machines','loafs','curtains','hours','taste','shaking','protest','needles','quicksand','battle','distance','bombs','hairs','smell'];

        var setup = function() {
            var defaultLayer = addNewLayer('Default layer');
            defaultLayer.data.isDefaultLayer = true;
            defaultLayer.data.id = getUniqueLayerID();

            ensureGuideLayer();

            defaultLayer.activate();

            if ( pg.layerPanel.onLayersChanged )
                pg.layerPanel.onLayersChanged();
        };


        var isLayer = function(item) {
            return item.className === "Layer";
        };

        var isActiveLayer = function(layer) {
            return paper.project.activeLayer.data.id == layer.data.id;
        };

        var getUniqueLayerID = function() {
            var biggestID = 0;
            for(var i=0; i<paper.project.layers.length; i++) {
                var layer = paper.project.layers[i];
                if(layer.data.id > biggestID) {
                    biggestID = layer.data.id;
                }
            }
            return biggestID + 1;
        };


        var ensureGuideLayer = function() {
            if(!getGuideLayer()) {
                var guideLayer = addNewLayer('pg.internalGuideLayer');
                guideLayer.data.isGuideLayer = true;
                guideLayer.data.id = getUniqueLayerID();
                guideLayer.bringToFront();
            }
        };


        var addNewLayer = function(layerName, setActive, elementsToAdd) {
            layerName = layerName || null;
            setActive = setActive || true;
            elementsToAdd = elementsToAdd || null;

            var newLayer = new paper.Layer();

            newLayer.data.id = getUniqueLayerID();

            if(layerName) {
                newLayer.name = layerName;
            } else {
                newLayer.name = 'Layer ' + newLayer.data.id;
            }

            if(setActive) {
                newLayer.activate();
            }

            if(elementsToAdd) {
                newLayer.addChildren(elementsToAdd);
            }

            var guideLayer = getGuideLayer();
            if(guideLayer) {
                guideLayer.bringToFront();
            }
            return newLayer;
        };


        var deleteLayer = function(id) {
            var layer = getLayerByID(id);
            if(layer) {
                layer.remove();
            }
            var defaultLayer = getDefaultLayer();
            if(defaultLayer) {
                defaultLayer.activate();
            }
        };


        var addItemsToLayer = function(items, layer) {
            layer.addChildren(items);
        };


        var addSelectedItemsToActiveLayer = function() {
            addItemsToLayer(pg.selection.getSelectedItems(), paper.project.activeLayer);
        };


        var getActiveLayer = function() {
            return paper.project.activeLayer;
        };


        var setActiveLayer = function(activeLayer) {
            activeLayer.activate();
        }


        var getLayerByID = function(id) {
            for(var i=0; i<paper.project.layers.length; i++) {
                var layer = paper.project.layers[i];
                if(layer.data.id == id) {
                    return layer;
                }
            }
            return false;
        };


        var getDefaultLayer = function() {
            for(var i=0; i<paper.project.layers.length; i++) {
                var layer = paper.project.layers[i];
                if(layer.data && layer.data.isDefaultLayer) {
                    return layer;
                }
            }
            return false;
        };


        var activateDefaultLayer = function() {
            var defaultLayer = getDefaultLayer();
            defaultLayer.activate();
        };


        var getGuideLayer = function() {
            for(var i=0; i<paper.project.layers.length; i++) {
                var layer = paper.project.layers[i];
                if(layer.data && layer.data.isGuideLayer) {
                    return layer;
                }
            }
            return false;
        };


        var getAllUserLayers = function() {
            var layers = [];
            for(var i=0; i<paper.project.layers.length; i++) {
                var layer = paper.project.layers[i];
                if(layer.data && layer.data.isGuideLayer) {
                    continue;
                }
                layers.push(layer);
            }
            return layers;
        };


        var changeLayerOrderByIDArray = function(order) {
            order.reverse();
            for(var i=0; i<order.length; i++) {
                getLayerByID(order[i]).bringToFront();
            }
            // guide layer is always top
            var guideLayer = getGuideLayer();
            if(guideLayer) {
                guideLayer.bringToFront();
            }
        };


        var reinitLayers = function(activeLayerID) {
            for(var i=0; i<paper.project.layers.length; i++) {
                var layer = paper.project.layers[i];
                if(layer.data.id == activeLayerID) {
                    pg.layer.setActiveLayer(layer);
                    break;
                }
            }
            /// TODO
//            pg.layerPanel.updateLayerList();
        };


        return {
            setup: setup,
            isLayer: isLayer,
            isActiveLayer: isActiveLayer,
            ensureGuideLayer: ensureGuideLayer,
            addNewLayer: addNewLayer,
            deleteLayer: deleteLayer,
            addItemsToLayer: addItemsToLayer,
            addSelectedItemsToActiveLayer: addSelectedItemsToActiveLayer,
            getActiveLayer: getActiveLayer,
            setActiveLayer: setActiveLayer,
            getLayerByID: getLayerByID,
            getDefaultLayer: getDefaultLayer,
            activateDefaultLayer: activateDefaultLayer,
            getGuideLayer: getGuideLayer,
            getAllUserLayers: getAllUserLayers,
            changeLayerOrderByIDArray: changeLayerOrderByIDArray,
            reinitLayers: reinitLayers
        };

    }();

    // functions related to selecting stuff

    pg.selection = function() {

        var getSelectionMode = function() {
            var activeTool = pg.toolbar.getActiveTool();
            if(activeTool) {
                var activeToolID = activeTool.options.id;
                if(activeToolID === 'detailselect') {
                    return 'Segment';
                } else {
                    return 'Item';
                }
            }
        };


        var selectAllItems = function() {
            var items = pg.document.getAllSelectableItems();

            for(var i=0; i<items.length; i++) {
                setItemSelection(items[i], true);
            }
        };


        var selectRandomItems = function() {
            var items = pg.document.getAllSelectableItems();

            for(var i=0; i<items.length; i++) {
                if(pg.math.getRandomBoolean()) {
                    setItemSelection(items[i], true);
                }
            }
        };




        var selectAllSegments = function() {
            var items = pg.document.getAllSelectableItems();

            for(var i=0; i<items.length; i++) {
                selectItemSegments(items[i], true);
            }
        };


        var selectItemSegments = function(item, state) {
            if(item.children) {
                for(var i=0; i<item.children.length; i++) {
                    var child = item.children[i];
                    if(child.children && child.children.length > 0) {
                        selectItemSegments(child, state);
                    } else {
                        child.fullySelected = state;
                    }
                }

            } else {
                for(var i=0; i<item.segments.length; i++) {
                    item.segments[i].selected = state;
                }
            }
        };


        var clearSelection = function() {
            paper.project.deselectAll();
            pg.hover.clearHoveredItem();
        };


        var invertItemSelection = function() {
            var items = pg.document.getAllSelectableItems();

            for(var i=0; i<items.length; i++) {
                items[i].selected = !items[i].selected;
            }
        };


        var invertSegmentSelection = function() {
            var items = pg.document.getAllSelectableItems();

            for(var i=0; i<items.length; i++) {
                var item = items[i];
                for(var j=0; j<item.segments.length; j++) {
                    var segment = item.segments[j];
                    segment.selected = !segment.selected;
                }
            }

            //jQuery(document).trigger('SelectionChanged');
        };


        var deleteSelection = function() {
            var selectionMode = getSelectionMode();

            if(selectionMode === 'Segment') {
                deleteSegmentSelection();
            } else {
                deleteItemSelection();
            }
        };


        var deleteItemSelection = function() {
            var items = getSelectedItems();
            for(var i=0; i<items.length; i++) {
                items[i].remove();
            }

            jQuery(document).trigger('DeleteItems');
            jQuery(document).trigger('SelectionChanged');
            paper.project.view.update();
            pg.undo.snapshot('deleteItemSelection');
        };


        var deleteSegmentSelection = function() {

            var items = getSelectedItems();
            for(var i=0; i<items.length; i++) {
                deleteSegments(items[i]);
            }

            jQuery(document).trigger('DeleteSegments');
            jQuery(document).trigger('SelectionChanged');
            paper.project.view.update();
            pg.undo.snapshot('deleteSegmentSelection');
        };


        var deleteSegments = function(item) {
            if(item.children) {
                for(var i=0; i<item.children.length; i++) {
                    var child = item.children[i];
                    deleteSegments(child);
                }
            } else {
                var segments = item.segments;
                for(var j=0; j<segments.length; j++) {
                    var segment = segments[j];
                    if(segment.selected) {
                        if(item.closed ||
                            (segment.next &&
                            !segment.next.selected &&
                            segment.previous &&
                            !segment.previous.selected) ) {

                            splitPathRetainSelection(item, j);
                            deleteSelection();
                            return;

                        } else if(!item.closed) {
                            segment.remove();
                            j--; // decrease counter if we removed one from the loop
                        }

                    }
                }
            }
            // remove items with no segments left
            if(item.segments.length <= 0) {
                item.remove();
            }
        };


        var splitPathAtSelectedSegments = function() {
            var items = getSelectedItems();
            for(var i=0; i<items.length; i++) {
                var item = items[i];
                var segments = item.segments;
                for(var j=0; j<segments.length; j++) {
                    var segment = segments[j];
                    if(segment.selected) {
                        if(item.closed ||
                            (segment.next &&
                            !segment.next.selected &&
                            segment.previous &&
                            !segment.previous.selected) ) {
                            splitPathRetainSelection(item, j, true);
                            splitPathAtSelectedSegments();
                            return;
                        }
                    }
                }
            }
        };


        var splitPathRetainSelection = function(path, index, deselectSplitSegments) {
            var selectedPoints = [];

            // collect points of selected segments, so we can reselect them
            // once the path is split.
            for(var i=0; i<path.segments.length; i++) {
                var seg = path.segments[i];
                if(seg.selected) {
                    if(deselectSplitSegments && i === index) {
                        continue;
                    }
                    selectedPoints.push(seg.point);
                }
            }

            var newPath = path.split(index, 0);
            if(!newPath) return;

            // reselect all of the newPaths segments that are in the exact same location
            // as the ones that are stored in selectedPoints
            for(var i=0; i<newPath.segments.length; i++) {
                var seg = newPath.segments[i];
                for(var j=0; j<selectedPoints.length; j++) {
                    var point = selectedPoints[j];
                    if(point.x === seg.point.x && point.y === seg.point.y) {
                        seg.selected = true;
                    }
                }
            }

            // only do this if path and newPath are different
            // (split at more than one point)
            if(path !== newPath) {
                for(var i=0; i<path.segments.length; i++) {
                    var seg = path.segments[i];
                    for(var j=0; j<selectedPoints.length; j++) {
                        var point = selectedPoints[j];
                        if(point.x === seg.point.x && point.y === seg.point.y) {
                            seg.selected = true;
                        }
                    }
                }
            }
        };


        var cloneSelection = function() {
            var selectedItems = getSelectedItems();
            for(var i = 0; i < selectedItems.length; i++) {
                var item = selectedItems[i];
                item.clone();
                item.selected = false;
            }
            pg.undo.snapshot('cloneSelection');

        };


        var setItemSelection = function(item, state) {
            var parentGroup = pg.group.getItemsGroup(item);
            var itemsCompoundPath = pg.compoundPath.getItemsCompoundPath(item);

            // if selection is in a group, select group not individual items
            if(parentGroup) {
                // do it recursive
                setItemSelection(parentGroup, state);

            } else if(itemsCompoundPath) {
                setItemSelection(itemsCompoundPath, state);

            } else {
                if(item.data && item.data.noSelect) {
                    return;
                }
                // fully selected segments need to be unselected first
                item.fullySelected = false;
                // then the item can be normally selected
                item.selected = state;
                // deselect children of compound-path or group for cleaner item selection
                if(pg.compoundPath.isCompoundPath(item) || pg.group.isGroup(item)) {

                    var children = item.children;
                    if(children) {
                        for(var i=0; i<children.length; i++) {
                            var child = children[i];
                            child.selected = !state;
                        }
                    }
                }
            }

            //TODO
//            pg.statusbar.update();
//            pg.stylebar.updateFromSelection();
//            pg.stylebar.blurInputs();

//            jQuery(document).trigger('SelectionChanged');

        };


        // this gets all selected non-grouped items and groups
        // (alternative to paper.project.selectedItems, which includes
        // group children in addition to the group)
        var getSelectedItems = function() {
            var allItems = paper.project.selectedItems;
            var itemsAndGroups = [];

            for(var i=0; i<allItems.length; i++) {
                var item = allItems[i];
                if(pg.item.isLayer(item)) {
                    continue;
                }
                if(pg.group.isGroup(item) &&
                    !pg.group.isGroup(item.parent) ||
                    !pg.group.isGroup(item.parent)) {
                    if(item.data && !item.data.isSelectionBound) {
                        itemsAndGroups.push(item);
                    }
                }
            }
            // sort items by index (0 at bottom)
            itemsAndGroups.sort(function(a, b) {
                    return parseFloat(a.index) - parseFloat(b.index);
            });
            return itemsAndGroups;
        };


        var getSelectionType = function() {
            var selection = getSelectedItems();
            if(selection.length === 0) {
                return false;
            }

            var selectionType = '';
            var lastSelectionType = '';
            for(var i=0; i<selection.length; i++) {
                var item = selection[i];
                if(getSelectionMode() === 'Segment') {
                    //todo: differentiate between segment, curve and handle
                    return 'Segment';
                }

                if(item.data.isPGTextItem) {
                    selectionType = 'Text';
                } else {
                    selectionType = item.className;
                }

                if(selectionType === lastSelectionType || lastSelectionType === '') {
                    lastSelectionType = selectionType;

                } else {
                    return 'Mixed';
                }
            }
            return selectionType;

        };


        // only returns paths, no compound paths, groups or any other stuff
        var getSelectedPaths = function() {
            var allPaths = getSelectedItems();
            var paths = [];

            for(var i=0; i<allPaths.length; i++) {
                var path = allPaths[i];
                if(path.className === 'Path') {
                    paths.push(path);
                }
            }
            return paths;
        };


        var switchSelectedHandles = function(mode) {
            var items = getSelectedItems();
            for(var i=0; i<items.length; i++) {
                var segments = items[i].segments;
                for(var j=0; j<segments.length; j++) {
                    var seg = segments[j];
                    if(!seg.selected) continue;

                    pg.geometry.switchHandle(seg, mode);
                }
            }
            pg.undo.snapshot('switchSelectedHandles');
        };


        var removeSelectedSegments = function() {
            pg.undo.snapshot('removeSelectedSegments');

            var items = getSelectedItems();
            var segmentsToRemove = [];

            for(var i=0; i<items.length; i++) {
                var segments = items[i].segments;
                for(var j=0; j < segments.length; j++) {
                    var seg = segments[j];
                    if(seg.selected) {
                        segmentsToRemove.push(seg);
                    }
                }
            }

            for(var i=0; i<segmentsToRemove.length; i++) {
                var seg = segmentsToRemove[i];
                seg.remove();
            }
        };


        var processRectangularSelection = function(event, rect, mode) {
            var allItems = pg.document.getAllSelectableItems();

            itemLoop:
            for(var i=0; i<allItems.length; i++) {
                var item = allItems[i];
                if(mode === 'detail' && pg.item.isPGTextItem(pg.item.getRootItem(item))) {
                    continue itemLoop;
                }
                // check for item segment points inside selectionRect
                if(pg.group.isGroup(item) || pg.item.isCompoundPathItem(item)) {
                    if(!rectangularSelectionGroupLoop(item, rect, item, event, mode)) {
                        continue itemLoop;
                    }

                } else {
                    if(!handleRectangularSelectionItems(item, event, rect, mode)) {
                        continue itemLoop;
                    }
                }
            }
        };


        // if the rectangular selection found a group, drill into it recursively
        var rectangularSelectionGroupLoop = function(group, rect, root, event, mode) {
            for(var i=0; i<group.children.length; i++) {
                var child = group.children[i];

                if(pg.group.isGroup(child) || pg.item.isCompoundPathItem(child)) {
                    rectangularSelectionGroupLoop(child, rect, root, event, mode);

                } else {
                    if(!handleRectangularSelectionItems(child, event, rect, mode)) {
                        return false;
                    }
                }
            }
            return true;
        };


        var handleRectangularSelectionItems = function(item, event, rect, mode) {
            if(pg.item.isPathItem(item)) {
                var segmentMode = false;

                // first round checks for segments inside the selectionRect
                for(var j=0; j<item.segments.length; j++) {
                    var seg = item.segments[j];
                    if( rect.contains(seg.point)) {
                        if(mode === 'detail') {
                            if(event.modifiers.shift && seg.selected) {
                                seg.selected = false;
                            } else {
                                seg.selected = true;
                            }
                            segmentMode = true;

                        } else {
                            if(event.modifiers.shift && item.selected) {
                                setItemSelection(item,false);

                            } else {
                                setItemSelection(item,true);
                            }
                            return false;
                        }
                    }
                }

                // second round checks for path intersections
                var intersections = item.getIntersections(rect);
                if( intersections.length > 0 && !segmentMode) {
                    // if in detail select mode, select the curves that intersect
                    // with the selectionRect
                    if(mode === 'detail') {
                        for(var k=0; k<intersections.length; k++) {
                            var curve = intersections[k].curve;
                            // intersections contains every curve twice because
                            // the selectionRect intersects a circle always at
                            // two points. so we skip every other curve
                            if(k % 2 === 1) {
                                continue;
                            }

                            if(event.modifiers.shift) {
                                curve.selected = !curve.selected;
                            } else {
                                curve.selected = true;
                            }
                        }

                    } else {
                        if(event.modifiers.shift && item.selected) {
                            setItemSelection(item,false);

                        } else {
                            setItemSelection(item,true);
                        }
                        return false;
                    }
                }

            } else if(pg.item.isBoundsItem(item)) {
                if(checkBoundsItem(rect, item, event)) {
                    return false;
                }
            }
            return true;
        };


        var checkBoundsItem = function(selectionRect, item, event) {
            var itemBounds = new paper.Path([
                item.localToGlobal(item.internalBounds.topLeft),
                item.localToGlobal(item.internalBounds.topRight),
                item.localToGlobal(item.internalBounds.bottomRight),
                item.localToGlobal(item.internalBounds.bottomLeft)
            ]);
            itemBounds.closed = true;
            itemBounds.guide = true;

            for(var i=0; i<itemBounds.segments.length; i++) {
                var seg = itemBounds.segments[i];
                if( selectionRect.contains(seg.point) ||
                    (i === 0 && selectionRect.getIntersections(itemBounds).length > 0)) {
                    if(event.modifiers.shift && item.selected) {
                        setItemSelection(item,false);

                    } else {
                        setItemSelection(item,true);
                    }
                    itemBounds.remove();
                    return true;

                }
            }

            itemBounds.remove();
        };


        return {
            getSelectionMode: getSelectionMode,
            selectAllItems: selectAllItems,
            selectRandomItems: selectRandomItems,
            selectAllSegments: selectAllSegments,
            clearSelection: clearSelection,
            invertItemSelection: invertItemSelection,
            invertSegmentSelection: invertSegmentSelection,
            deleteSelection: deleteSelection,
            deleteItemSelection: deleteItemSelection,
            deleteSegmentSelection: deleteSegmentSelection,
            splitPathAtSelectedSegments: splitPathAtSelectedSegments,
            cloneSelection: cloneSelection,
            setItemSelection: setItemSelection,
            getSelectedItems: getSelectedItems,
            getSelectionType: getSelectionType,
            getSelectedPaths: getSelectedPaths,
            switchSelectedHandles: switchSelectedHandles,
            removeSelectedSegments: removeSelectedSegments,
            processRectangularSelection: processRectangularSelection
        };

    }();



    pg.compoundPath = function() {


        var isCompoundPath = function(item) {
            return item && item.className === 'CompoundPath';
        };

        var isCompoundPathChild = function(item) {
            if(item.parent) {
                return item.parent.className === 'CompoundPath';
            } else {
                return false;
            }
        };


        var getItemsCompoundPath = function(item) {
            var itemParent = item.parent;

            if(isCompoundPath(itemParent)) {
                return itemParent;
            } else {
                return null;
            }

        };


        var createFromSelection = function() {
            var items = pg.selection.getSelectedPaths();
            if(items.length < 2) return;

            var path = new paper.CompoundPath({fillRule: 'evenodd'});

            for(var i=0; i<items.length; i++) {
                path.addChild(items[i]);
                items[i].selected = false;
            }

            path = pg.stylebar.applyActiveToolbarStyle(path);

            pg.selection.setItemSelection(path, true);
            pg.undo.snapshot('createCompoundPathFromSelection');
        };


        var releaseSelection = function() {
            var items = pg.selection.getSelectedItems();

            var cPathsToDelete = [];
            for(var i=0; i<items.length; i++) {
                var item = items[i];

                if(isCompoundPath(item)) {

                    for(var j=0; j<item.children.length; j++) {
                        var path = item.children[j];
                        path.parent = item.layer;
                        pg.selection.setItemSelection(path, true);
                        j--;
                    }
                    cPathsToDelete.push(item);
                    pg.selection.setItemSelection(item, false);

                } else {
                    items[i].parent = item.layer;
                }
            }

            for(var j=0; j<cPathsToDelete.length; j++) {
                cPathsToDelete[j].remove();
            }
            pg.undo.snapshot('releaseCompoundPath');
        };


        return {
            isCompoundPath: isCompoundPath,
            isCompoundPathChild: isCompoundPathChild,
            getItemsCompoundPath: getItemsCompoundPath,
            createFromSelection: createFromSelection,
            releaseSelection: releaseSelection
        };
    }();


    pg.helper = function() {

        var selectedItemsToJSONString = function() {
            var selectedItems = pg.selection.getSelectedItems();
            if(selectedItems.length > 0) {
                var jsonComp = '[["Layer",{"applyMatrix":true,"children":[';
                for(var i=0; i < selectedItems.length; i++) {
                    var itemJSON = selectedItems[i].exportJSON({asString: true});
                    if(i+1 < selectedItems.length) {
                        itemJSON += ',';
                    }
                    jsonComp += itemJSON;
                }
                return jsonComp += ']}]]';
            } else {
                return null;
            }
        };


        var getAllPaperItems = function(includeGuides) {
            includeGuides = includeGuides || false;
            var allItems = [];
            for(var i=0; i<paper.project.layers.length; i++) {
                var layer = paper.project.layers[i];
                for(var j=0; j<layer.children.length; j++) {
                    var child = layer.children[j];
                    // don't give guides back
                    if(!includeGuides && child.guide) {
                        continue;
                    }
                    allItems.push(child);
                }
            }
            return allItems;
        };


        var getPaperItemsByLayerID = function(id) {
            var allItems = getAllPaperItems(false);
            var foundItems = [];


            for ( var index = 0; index < allItems.length; ++index ){
                var item = allItems[index]
                if(item.layer.data.id === id) {
                    foundItems.push(item);
                }
            }

            return foundItems;
        };


        var getPaperItemsByTags = function(tags) {
            var allItems = getAllPaperItems(true);
            var foundItems = [];

            for ( var index = 0; index < allItems.length; ++index ){
                var item = allItems[index]
                for ( var ti = 0; ti < tags.length; ++ti ){
                    var tag = tags[ti]
                    if(item[tag] && foundItems.indexOf(item) === -1) {
                        foundItems.push(item);
                    }
                }
            }
            return foundItems

        };


        var removePaperItemsByDataTags = function(tags) {
            var allItems = getAllPaperItems(true);
            for ( var index = 0; index < allItems.length; ++index ){
                var item = allItems[index]
                for ( var ti = 0; ti < tags.length; ++ti ){
                    var tag = tags[ti]
                    if(item.data && item.data[tag]) {
                        item.remove();
                    }
                }
            }
        };


        var removePaperItemsByTags = function(tags) {
            var allItems = getAllPaperItems(true);
            for ( var index = 0; index < allItems.length; ++index ){
                var item = allItems[index]
                for ( var ti = 0; ti < tags.length; ++ti ){
                    var tag = tags[ti]
                    if(item[tag]) {
                        item.remove();
                    }
                }
            }
        };


        var processFileInput = function (dataType ,input, callback) {
            var reader = new FileReader();

            if(dataType === 'text') {
                reader.readAsText(input.files[0]);

            } else if(dataType === 'dataURL') {
                reader.readAsDataURL(input.files[0]);
            }

            reader.onload = function () {
                callback(reader.result);
            };
        };


        var executeFunctionByName = function (functionName, context /*, args */) {
            var args = [].slice.call(arguments).splice(2);
            var namespaces = functionName.split(".");
            var func = namespaces.pop();
            for (var i = 0; i < namespaces.length; i++) {
                context = context[namespaces[i]];
            }
            return context[func].apply(context, args);
        };


        return {
            selectedItemsToJSONString: selectedItemsToJSONString,
            getAllPaperItems: getAllPaperItems,
            getPaperItemsByLayerID: getPaperItemsByLayerID,
            getPaperItemsByTags: getPaperItemsByTags,
            removePaperItemsByDataTags: removePaperItemsByDataTags,
            removePaperItemsByTags: removePaperItemsByTags,
            processFileInput:processFileInput,
            executeFunctionByName: executeFunctionByName
        };

    }();

    // undo functionality
    // slightly modifed from https://github.com/memononen/stylii

    pg.undo = function() {
        var states = [];
        var head = -1;
        var maxUndos = 80;

        var setup = function() {
            pg.undo.snapshot('init');
        };

        var snapshot = function(type) {
            var state = {
                type : type,
                json : paper.project.exportJSON({ asString: false})
            };

            // remove all states after the current head
            if(head < states.length-1) {
                states = states.slice(0, head+1);
            }

            // add the new states
            states.push(state);

            // limit states to maxUndos by shifing states (kills first state)
            if(states.length > maxUndos) {
                states.shift();
            }

            // set the head to the states length
            head = states.length-1;

        };


        var undo = function() {
            if(head > 0) {
                head--;
                restore(states[head]);
            }
        };


        var redo = function() {
            if(head < states.length-1) {
                head++;
                restore(states[head]);
            }
        };


        var removeLastState = function() {
            states.splice(-1, 1);
        };


        var restore = function(entry) {
            var activeLayerID = paper.project.activeLayer.data.id;
            paper.project.clear();
            paper.view.update();
            paper.project.importJSON(entry.json);
            pg.layer.reinitLayers(activeLayerID);
        };


        var clear = function() {
            states = [];
            head = -1;
        };


        var getStates = function() {
            return states;
        };


        var getHead = function() {
            return head;
        };


        return {
            setup: setup,
            snapshot: snapshot,
            undo: undo,
            redo: redo,
            removeLastState: removeLastState,
            clear: clear,
            getStates: getStates,
            getHead: getHead
        };
    }();


    pg.document = function() {
        var center;
        var clipboard = [];

        var setup = function() {
//            paper.view.center = new paper.Point(0,0);
//            center = paper.view.center;

            // call DocumentUpdate at a reduced rate (every tenth frame)
//            var int = 10;
//            paper.view.onFrame = function() {
//                if(int > 0) {
//                    int--;
//                } else {
//                    jQuery(document).trigger('DocumentUpdate');
//                    int = 10;
//                }
//            };


//            window.onbeforeunload = (function(e) {
//                if(pg.undo.getStates().length > 1) {
//                    return 'Unsaved changes will be lost. Leave anyway?';
//                }
//            });
        };


        var clear = function() {
            paper.project.clear();
            pg.undo.clear();
            setup();
            pg.layer.setup();
        };


        var getCenter = function() {
            return center;
        };


        var getClipboard = function() {
            return clipboard;
        };


        var pushClipboard = function(item) {
            clipboard.push(item);
            return true;
        };


        var clearClipboard = function() {
            clipboard = [];
            return true;
        };


        var getAllSelectableItems = function() {
            var allItems = pg.helper.getAllPaperItems();
            var selectables = [];
            for(var i=0; i<allItems.length; i++) {
                if(allItems[i].data && !allItems[i].data.isHelperItem) {
                    selectables.push(allItems[i]);
                }
            }
            return selectables;
        };


        var loadJSONDocument = function(jsonString) {
            var activeLayerID = paper.project.activeLayer.data.id;
            paper.project.clear();
            pg.toolbar.setDefaultTool();
            pg.export.setExportRect();

            paper.project.importJSON(jsonString);

            pg.layer.reinitLayers(activeLayerID);

            var exportRect = pg.guides.getExportRectGuide();
            if(exportRect) {
                pg.export.setExportRect(new paper.Rectangle(exportRect.data.exportRectBounds));
            }
            pg.undo.snapshot('loadJSONDocument');
        };


        var saveJSONDocument = function() {
            var fileName = prompt("Name your file", "export.json");

            if (fileName !== null) {
                pg.hover.clearHoveredItem();
                pg.selection.clearSelection();
                paper.view.update();

                var fileNameNoExtension = fileName.split(".json")[0];
                var exportData = paper.project.exportJSON({ asString: true });
                var blob = new Blob([exportData], {type: "text/json"});
                saveAs(blob, fileNameNoExtension+'.json');
            }
        };


        return {
            getCenter: getCenter,
            setup: setup,
            clear: clear,
            getClipboard: getClipboard,
            pushClipboard: pushClipboard,
            clearClipboard: clearClipboard,
            getAllSelectableItems: getAllSelectableItems,
            loadJSONDocument: loadJSONDocument,
            saveJSONDocument: saveJSONDocument
        };

    }();

    // functions related to math

    pg.math = function() {

        var checkPointsClose = function(startPos, eventPoint, threshold) {
            var xOff = Math.abs(startPos.x - eventPoint.x);
            var yOff = Math.abs(startPos.y - eventPoint.y);
            if(xOff < threshold && yOff < threshold) {
                return true;
            }
            return false;
        };


        var getRandomInt = function(min, max) {
            return Math.floor(Math.random() * (max - min)) + min;
        };


        var getRandomBoolean = function() {
            return getRandomInt(0,2) === 1 ? false : true;
        };


        // Thanks Mikko Mononen! https://github.com/memononen/stylii
        var snapDeltaToAngle = function(delta, snapAngle) {
            var angle = Math.atan2(delta.y, delta.x);
            angle = Math.round(angle/snapAngle) * snapAngle;
            var dirx = Math.cos(angle);
            var diry = Math.sin(angle);
            var d = dirx*delta.x + diry*delta.y;
            return new paper.Point(dirx*d, diry*d);
        };


        return {
            checkPointsClose:checkPointsClose,
            getRandomInt:getRandomInt,
            getRandomBoolean: getRandomBoolean,
            snapDeltaToAngle:snapDeltaToAngle
        };

    }();

    pg.text = function() {
        var importedFonts = [];

        var setup = function() {
            jQuery.getJSON('fonts/fonts.json', function(data){
                jQuery.each(data.fonts, function(index, fontName) {
                    opentype.load('fonts/'+fontName, function (err, font) {
                        if (err) {
                            console.error(err);

                        } else {
                            importFontLoaded(font);
                        }
                    });
                });
            });
        };


        var addImportedFont = function(fontName, fontStyle, fontToAdd) {
            // check if font with the same name already exists. if it does, just
            // add the style (or overwrite it, if the style already exists)
            for(var i=0; i<importedFonts.length; i++) {
                var font = importedFonts[i];
                if(font.name == fontName) {
                    for(var j=0; j<font.styles.length; j++) {
                        var fStyle = font.styles[j];
                        if(fStyle.style == fontStyle) {
                            fStyle.font = fontToAdd;
                            return true;
                        }
                    }
                    // if the style wasn't found, add it
                    font.styles.push({
                        'style': fontStyle,
                        'font' : fontToAdd
                    });
                    return true;
                }
            }
            // if no font with that name existed, add it as new
            importedFonts.push({
                'name': fontName,
                'styles': [{
                    'style': fontStyle,
                    'font': fontToAdd
                }]
            });
            return true;
        };


        var getImportedFonts = function() {
            return importedFonts;
        };


        var getImportedFont = function(fontName, fontStyle) {
            for (var i=0; i<importedFonts.length; i++) {
                var font = importedFonts[i];
                if(font.name == fontName) {
                    for(var j=0; j<font.styles.length; j++) {
                        var style = font.styles[j];
                        if(style.style == fontStyle) {
                            return style.font;
                        }
                    }
                }
            }
        };


        var readFontFilesFromInput = function(e, doneCallback) {
            var files = e.target.files;
            for (var i = 0; i < files.length; i++) {
                var reader = new FileReader();
                reader.readAsArrayBuffer(files[i]);

                reader.onload = function (e) {
                    try {
                        var font = opentype.parse(e.target.result);
                        importFontLoaded(font);
                        doneCallback(font);
                    } catch (err) {
                        console.error(err.toString());
                    }
                };
                reader.onerror = function (err) {
                    console.error(err.toString());
                };
            }
        };


        var importFontLoaded = function(font) {
            var info = getShortInfoFromFont(font);
            addImportedFont(info.fontFamily, info.fontStyle, font);
        };


        var getShortInfoFromFont = function(font) {
            var info = {};

            if(font.names.preferredFamily) {
                info.fontFamily = font.names.preferredFamily.en;
            } else {
                info.fontFamily = font.names.fontFamily.en;
            }

            if(font.names.preferredSubfamily) {
                info.fontStyle = font.names.preferredSubfamily.en;
            } else {
                info.fontStyle = font.names.fontSubfamily.en;
            }
            return info;
        };


        var createPGTextItem = function(text, options, pos) {
            var font = getImportedFont(options.fontFamily, options.fontStyle);
            var scaleFactor = options.fontSize/font.unitsPerEm;
            var asc = font.ascender;
            var desc = font.descender;
            var glyphs = font.stringToGlyphs(text);
            var offsetX = 0;
            var textGroup = new paper.Group();
            var lastGlyphIndex = -1;

            for ( var index = 0; index < glyphs.length; ++index ){
                var glyph = glyphs[index]

                var kerningPairValue = 0;
                if(lastGlyphIndex >= 0) {
                    var kerning = font.kerningPairs[lastGlyphIndex+','+glyph.index];
                    if(kerning !== undefined) {
                        kerningPairValue = kerning;
                    }
                }
                var glyphGroup = new paper.Group();
                var glyphRect = new paper.Rectangle(new paper.Point(0, -asc), new paper.Point(glyph.advanceWidth, desc * -1));
                var glyphRectPaperPath = new paper.Path.Rectangle(glyphRect);
                glyphRectPaperPath.data.isPGGlyphRect = true;
                glyphRectPaperPath.fillColor = 'rgba(0,0,0,0.00001)';
                glyphGroup.addChild(glyphRectPaperPath);
                glyphGroup.data.isPGGlyphGroup = true;

                var glyphPath = glyph.getPath(0, 0, font.unitsPerEm);
                var glyphPaperPath = new paper.CompoundPath(glyphPath.toPathData());


                //TODO: Apply toolbar style
                // ------------------------

//                pg.stylebar.applyActiveToolbarStyle(glyphPaperPath);
                glyphPaperPath.fillColor = 'black';

                glyphPaperPath.opacity = 1;
                glyphPaperPath.blendMode = 'normal';

                glyphGroup.addChild(glyphPaperPath);

                glyphGroup.pivot = glyphRectPaperPath.bounds.topLeft;
                glyphGroup.position.x = offsetX + kerningPairValue + options.letterSpacing * 10;
                textGroup.addChild(glyphGroup);
                offsetX += glyphRectPaperPath.bounds.width - (kerningPairValue*-1) - (options.letterSpacing * 10 *-1);
                lastGlyphIndex = glyph.index;
            };

            textGroup.scale(scaleFactor);
            textGroup.position = new paper.Point(pos.x, pos.y);
            textGroup.data.isPGTextItem = true;
            textGroup.data.text = text;
            textGroup.data.fontFamily = options.fontFamily;
            textGroup.data.fontStyle = options.fontStyle;
            textGroup.data.fontSize = options.fontSize;
            textGroup.data.letterSpacing = options.letterSpacing;

            return textGroup;
        };


        var convertSelectionToOutlines = function() {
            var items = pg.selection.getSelectedItems();
            for(var i=0; i<items.length; i++) {
                var outlines = [];
                var item = items[i];
                var opacity = item.opacity;
                var blendMode = item.blendMode;
                if(pg.item.isPGTextItem(item)) {
                    for(var j=0; j<item.children.length; j++) {
                        var child = item.children[j];
                        if(child.data.isPGGlyphGroup) {
                            for(var k=0; k<child.children.length; k++) {
                                if(!child.children[k].data.isPGGlyphRect) {
                                    outlines.push(child.children[k]);
                                }
                            }
                        }
                    }
                    item.remove();
                    var group = new paper.Group();
                    pg.layer.getActiveLayer().addChild(group);
                    group.addChildren(outlines);
                    group.opacity = opacity;
                    group.blendMode = blendMode;
                    pg.selection.setItemSelection(group, true);
                }
            }
        };


        return {
            setup: setup,
            addImportedFont: addImportedFont,
            getImportedFonts: getImportedFonts,
            getImportedFont: getImportedFont,
            getShortInfoFromFont: getShortInfoFromFont,
            createPGTextItem: createPGTextItem,
            convertSelectionToOutlines: convertSelectionToOutlines,
            readFontFilesFromInput: readFontFilesFromInput
        };

    }();

    pg.import = function(){

        var importAndAddSVG = function (svgString) {
            paper.project.importSVG(svgString, {expandShapes: true});
            pg.undo.snapshot('importAndAddSVG');
        };

        var importAndAddImage = function (src) {
            var raster = new paper.Raster({
                source: src,
                position: paper.view.center
            });
            pg.undo.snapshot('importImage');
        };
    }();

}
