// $Id$

/**
 * Ting facet browser configuration function.
 */
Drupal.tingFacetBrowser = function(facetBrowserElement, searchResultElement, result) {
  this.searchResultElement = searchResultElement;
  this.facetBrowserElement = facetBrowserElement;

  this.renderFacetBrowser = function(element, result) {
    var facets, facetBrowser, facetGroups, facetTerms, maxFacets, numFacets, template;
    //The base markup for the browser
    template = $('<ol class="facet-groups">'+
                        '<li class="facet-group">'+
                          '<h4></h4>'+
                          '<ol class="facets">'+
                            '<li><span class="name"></span>(<span class="count"></span>)</li>'+
                          '</ol>'+
                        '</li>'+
                      '</ol>');

    facetGroups = $(template).mapDirective({
      '.facet-group': 'facet <- facets',
      '.facet-group[facet-group]': 'facet.name',
      '.facet-group[class]+': function(arg) {
        var facets, firstLast;
        facets = Object.keys(arg.items);
        firstLast = (facets[0] == arg.item.name) ? ' first' :
                    ((facets[facets-length-1] == arg.item.name) ? ' last' : '');
        return firstLast;
      },
      'h4': function(arg) { return (Drupal.settings.tingResult.facetNames[arg.item.name]) ? Drupal.settings.tingResult.facetNames[arg.item.name] : arg.item.name; }
    });

    facetTerms = $('.facets', facetGroups).mapDirective({
      'li': 'term <- facet.terms',
      'li[class]+': function(arg) {
        return ((($.inArray(arg.pos.toString(), Object.keys(arg.items)) % 2) === 0) ? ' odd' : ' even' );
      },
      'li[facet]': function(arg) { return arg.pos; },
      'li[facet-group]': 'facet.name',
      '.name': function(arg) { return arg.pos; },
      '.count': function(arg) { return arg.item; }
    });

    $('.facets', facetGroups).html($('li', facetTerms));
    $p.compile(facetGroups, 'facet-groups');

    facetBrowser = $($p.render('facet-groups', result));

    //Add additional empty list items to make evenly sized lists
    facets = $('.facets', facetBrowser);
    numFacets = facets.map(function(i, e) {
      return $('li', e).size();
    });
    maxFacets = Math.max.apply(Math, $.makeArray(numFacets));

    facets.each(function(i, e) {
      var facetElement = $('li:first', e);
      for (i = $('li', e).size(); i < maxFacets; i++) {
        $(e).append(facetElement.clone().removeClass().addClass((((i % 2) === 0) ? 'odd' : 'even')).addClass('hidden'));
      }
    });

    $(element).html(facetBrowser);

    this.resizeFacets(element);
  };

  this.updateFacetBrowser = function(element, result) {
    for (f in result.facets) {
      var facetElements, selectedTerms;
      facetElements = $('.facet-group[facet-group='+f+']', element);
      selectedTerms = $('.selected', facetElements).map(function(i, e) {
        return $(e).attr('facet');
      });

      $.each(Object.keys(result.facets[f].terms), function (i, t) {
        var facetElement = $('li:eq('+i+')', facetElements);
        //update facet values
        facetElement.attr('facet', t);
        facetElement.find('.name').text(t);
        facetElement.find('.count').text(result.facets[f].terms[t]);

        //display facet and determine whether it is selected
        facetElement.removeClass('hidden');
        ((facetElement.size() > 0) && $.inArray(t, selectedTerms) > -1) ?  facetElement.addClass('selected') : facetElement.removeClass('selected');
      });

      //hide and unselect all superflous facets
      $('li:gt('+(Object.keys(result.facets[f].terms).length-1)+')', facetElements).removeClass('selected').addClass('hidden');
    }

    this.resizeFacets(element);
  };

  this.resetFacetBrowser = function(element) {
    var resize, width;
    element = $(element);

    width = 0;
    element.find('.jcarousel-item').each(function() {
      width = width + $(this).width();
    });
    element.find('.jcarousel-list').width(width);

    resize = element.find('.resize');
    (element.height() < Drupal.getFacetHeight(element)) ? resize.addClass('expand') : resize.addClass('contract');
  };

  this.resizeFacets = function(element) {
    setTimeout(function() {
      $('.facets:first li', element).each(function(i, e) {
        var facets, heights, maxHeight;
        //Locate the height of the tallest element in the row
        facets = $('.facets li:nth-child('+(i+1)+')');
        facets.css({ paddingTop: '', paddingBottom: '' });
        heights = facets.map(function(i, e) { return $(e).height(); });
        maxHeight = Math.max.apply(Math, $.makeArray(heights));

        //Find smallers element in the row then add the difference as top and
        //bottom padding to center the content vertically
        facets.each(function(i, e) {
          var padding;
          if ($(e).height() < maxHeight) {
            padding = ((maxHeight-$(e).height())/2);
            $(e).css({ paddingTop: padding+'px',
                            paddingBottom: padding+'px' });
          }
        });
      });
    }, 0);
  };

  this.initCarousel = function(element) {
    $(element).children().addClass('jcarousel-skin-ding-facet-browser').jcarousel();
  };

  this.renderResizeButton = function(element) {
    var button = $('<a class="resize" href="">'+Drupal.settings.tingResult.resizeButtonText+'</a>');
    element = $(element);
    (element.height() < Drupal.getFacetHeight(element)) ? button.addClass('expand') : button.addClass('disabled');

    $(element).append(button);
  };

  this.bindResizeEvent = function(element) {
    var baseHeight, headerHeight, resizeButton;
    element = $(element);
    baseHeight = element.outerHeight();
    headerHeight = $('H4', element).height();
    resizeButton = $('.resize', element);

    $('.expand', $(element)).toggle(function() {
      resizeButton.css('top', resizeButton.position().top+'px');
      $('.jcarousel-clip-horizontal', element).animate({ 'height': (Drupal.getFacetHeight()+headerHeight)+'px' }, 1000, 'swing', function()
      {
        $('.resize', element).toggleClass('expand').toggleClass('contract');
      });
      $('.resize', element).animate({ 'top': (Drupal.getFacetHeight()+headerHeight+6)+'px' }, 1000, 'swing');
      $('.jcarousel-prev, .jcarousel-next', element).animate({ 'top': ((Drupal.getFacetHeight()+headerHeight)/2)+'px' }, 1000, 'swing');

    }, function() {
      $('.jcarousel-clip-horizontal', element).animate({ 'height': baseHeight+'px' }, 1000, 'swing', function() {
        $('.resize', element).toggleClass('expand').toggleClass('contract');
      });
      $('.resize', element).animate({ 'top': (baseHeight)+'px' }, 1000, 'swing' );
      $('.jcarousel-prev, .jcarousel-next', element).animate({ 'top': (baseHeight/2)+'px' }, 1000, 'swing');
    });
  };

  this.bindSelectEvent = function(facetBrowserElement, searchResultElement) {
    var $facetBrowser = $(facetBrowserElement);
    $facetBrowser.find('.facets li').unbind('click');
    $facetBrowser.find('.facets li:not(.hidden)').click(function() {
      $(this).toggleClass('selected');
      Drupal.updateSelectedUrl(facetBrowserElement);
      Drupal.doUrlSearch(facetBrowserElement, searchResultElement);
    });
  };

  this.updateSelectedUrl = function(element) {
    var facets, sort, vars;
    facets = '';
    $('.selected', element).each(function(i, e) {
      facets += $(e).attr('facet-group')+':'+$(e).attr('facet')+';';
    });
    sort = Drupal.getAnchorVars().sort;
    vars = {};
    if (facets.length > 0) {
      vars.facets = facets;
    }
    if (sort) {
      vars.sort = sort;
    }
    Drupal.setAnchorVars(vars);
  };

  this.updateSelectedFacetsFromUrl = function(element) {
    var facets, match;
    if ($.url.attr('anchor')) {
      match = $.url.attr('anchor').match('facets=(([^:]*:[^;]*;)+)');
      if (match && match.length > 1) {
        facets = match[1].split(';');
        for (f in facets) {
          f = facets[f].split(':');
          if (f.length > 1) {
            $('[facet-group='+f[0]+'][facet='+f[1]+']:not(.hidden)', element).addClass('selected');
          }
        }
      }
    }
    return $('.selected', element).size() > 0;
  };

  this.getFacetHeight = function(element) {
    var maxHeight = 0;
    $('.facets', element).each(function() {
      maxHeight = Math.max(maxHeight, $(this).height());
    });
    return maxHeight;
  };

  //initialization
  this.renderFacetBrowser(facetBrowserElement, result);
  if (this.updateSelectedFacetsFromUrl(facetBrowserElement)) {
    this.doUrlSearch(facetBrowserElement, searchResultElement);
  }

  this.initCarousel(facetBrowserElement);
  this.renderResizeButton(facetBrowserElement);
  this.bindResizeEvent(facetBrowserElement);
  this.bindSelectEvent(facetBrowserElement, searchResultElement);
};

