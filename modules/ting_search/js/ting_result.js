// $Id$

/**
 * Search result handling function.
 */
Drupal.tingResult = function (searchResultElement, facetBrowserElement, result) {
  this.searchResultElement = searchResultElement;
  this.facetBrowserElement = facetBrowserElement;

  /**
   * Render the search results.
   */
  this.renderTingSearchResults = function (element, result) {
    var $element = $(element);
    $element.find('ul,ol').html(result.result_html);
    Drupal.tingSearch.updateSummary($('#ting-search-summary'), result);

    // Append information the current query and where the current
    // result appears in this to each link
    // Build a string containing the query elements: Keys and selected facets
    var vars = Drupal.getAnchorVars();
    vars.query = Drupal.settings.tingSearch.keys;
    var var_string = '';
    for (key in vars) {
      if (key != 'page') { //Ignore page number - entry number is calculated later
        var_string += encodeURIComponent(key + ':' + vars[key] + ';');
      }
    }

    var entries = $element.find('.record a.title');
    entries.each(function(i) {
      // Calculate where the current item appears in the result
      var page = (vars.page) ? vars.page : 1;
      var entry = (entries.length * (page - 1)) + (i + 1);
      var entry_string = encodeURIComponent('entry:'+entry);

      //Append query and entry information to the
      $(this).attr('href', $(this).attr('href')+'/'+var_string+entry_string);
		});

    // If possible, look up availability from backend provider.
    if (Drupal.hasOwnProperty('tingAvailability')) {
      Drupal.tingAvailability.id_list = result.local_ids;
      Drupal.tingAvailability.get_availability(function (data, textStatus) {
        if (!data) { return; }
        var $list = $element.find('ul.ting-search-collection-types');

        // For each ID, find the associated type indicators and set
        // their status according to the data from the provider.
        $.each(data, function (providerID, available) {
          var $type = $list.find('.ting-object-' + providerID);
          // If it already has the available class, don't touch it.
          if (!$type.hasClass('available')) {
            if (available) {
              $type.addClass('available');
              $type.removeClass('unavailable');
            }
            else {
              $type.addClass('unavailable');
            }
          }
        });
      });
    }
  };

  /**
   * Render the search results pager.
   */
  this.renderTingSearchResultPager = function (element, result) {
    var anchorVars, morePages, currentPage, $pager, pageNumberClasses;
    anchorVars = Drupal.getAnchorVars();
    morePages = (result.collectionCount >= result.resultsPerPage);
    currentPage = (anchorVars.page && !isNaN(parseInt(anchorVars.page, 10))) ? parseInt(anchorVars.page, 10) : 1;

    // Don't bother with a pager if there is nothing to paginate to.
    if (morePages || currentPage > 1) {
      $pager = $(Drupal.settings.tingResult.pagerTemplate);

      // If we're on the first page, remove the previous  and first page
      // links from the template.
      if (currentPage < 2) {
        $pager.find('a.prev').parent().remove();
        $pager.find('a.first').parent().remove();
      }

      // If there's no more pages, remove the next link.
      if (!morePages) {
        $pager.find('a.next').parent().remove();
      }

      pageNumberClasses = {
        '.first': 1,
        '.prev': currentPage - 1,
        '.next': currentPage + 1
      };

      $.each(pageNumberClasses, function(i, e) {
        var page = pageNumberClasses[i];
        $pager.find(i).click(function() {
          Drupal.updatePageUrl(page);
          Drupal.doUrlSearch(facetBrowserElement, searchResultElement);
          jQuery.scrollTo($('#ting-search-results').get(0));
          return false;
        });
      });

      $($pager).find('ul a').click(function() {
        Drupal.updatePageUrl($(this).text());
        Drupal.doUrlSearch(facetBrowserElement, searchResultElement);
        jQuery.scrollTo($('#ting-search-results').get(0));
        return false;
      });

      // Kasper: What is the purpose of this?
      element = $(element);
      if (element.next('#pager').size() > 0) {
        element.next('#pager').replaceWith($pager);
      }
      else {
        element.after($pager);
      }
    }
  };

  /**
   * Render the search results feed icon.
   */
  this.renderTingSearchResultFeedIcon = function (element, result) {
      $element = $(element);
      if (result['feed_icon']) {
        if ($element.nextAll('.feed-icon').size() > 0) {
          $element.nextAll('.feed-icon').replaceWith(result['feed_icon']);
        }
        else {
          $element.parent().append(result['feed_icon']);
        }
      }
  };

  /**
   * Perform search based on the #-anchor in the URL.
   */
  this.doUrlSearch = function() {
      //Start loading
      Drupal.tingSearch.tabLoading('ting');

      var vars = Drupal.getAnchorVars();
      vars.query = Drupal.settings.tingSearch.keys;

      var url = Drupal.settings.tingSearch.ting_url;

      $.getJSON(url, vars, function(data) {
        //Update tabs now that we have the result
        Drupal.tingSearch.summary.ting = { count: data.count, page: data.page };
        Drupal.tingSearch.updateTabs('ting');

        //Update search result and facet browser
        Drupal.renderTingSearchResults(Drupal.searchResultElement, data);
        Drupal.renderTingSearchResultPager(Drupal.searchResultElement, data);
        Drupal.renderTingSearchResultFeedIcon(searchResultElement, result);
        Drupal.updateFacetBrowser(Drupal.facetBrowserElement, data);
        Drupal.bindSelectEvent(Drupal.facetBrowserElement, searchResultElement);
        Drupal.updateSelectedFacetsFromUrl(Drupal.facetBrowserElement);
      });
  };

  /**
   * Update the #-anchor in the URL.
   *
   * Sets the current page and search parameters.
   */
  this.updatePageUrl = function(pageNumber) {
    var anchorVars = Drupal.getAnchorVars();
    anchorVars.page = pageNumber;
    Drupal.setAnchorVars(anchorVars);
  };

  this.updateSortUrl = function(sort) {
    var anchorVars = Drupal.getAnchorVars();
    anchorVars.sort = sort;
    delete anchorVars.page;
    Drupal.setAnchorVars(anchorVars);
  };

  this.renderTingSearchResults(searchResultElement, result);
  this.renderTingSearchResultPager(searchResultElement, result);
  this.renderTingSearchResultFeedIcon(searchResultElement, result);

  $('#edit-ting-search-sort').val(Drupal.getAnchorVars().sort);
  $('#edit-ting-search-sort').change(function() {
    Drupal.updateSortUrl($(this).val());
    Drupal.doUrlSearch(facetBrowserElement, searchResultElement);
  });
};

