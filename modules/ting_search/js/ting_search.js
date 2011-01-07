/* $Id$ */

/**
 * @file ting_search.js
 * JavaScript file holding most of the ting search related functions.
 */

/**
 * Set up the results page.
 *
 * This is _not_ a Drupal.behavior, since those take a lot longer to load.
 */
$(function () {
  $("#search :text")
    // Put the search keys into the main searchbox.
    .val(Drupal.settings.tingSearch.keys)
    // And trigger the change event so that InFieldLabes will work correctly.
    .change();

  // Configure our tabs
  $("#ting-search-tabs")
    .tabs( { select: Drupal.tingSearch.selectTab } )
    // Disable the website tab until we have some results.
    .tabs("disable", 1);

  Drupal.tingSearch.getTingData(Drupal.settings.tingSearch.ting_url,
                                Drupal.settings.tingSearch.keys);

  Drupal.tingSearch.getContentData(Drupal.settings.tingSearch.content_url,
                                   Drupal.settings.tingSearch.keys);
});

// Container object
Drupal.tingSearch = {
  // Holds the number of results for each of the different types of search.
  summary: {}
};

// Get search data from Ting
Drupal.tingSearch.getTingData = function(url, keys) {
  var vars = Drupal.getAnchorVars();
  vars.query = keys;

  $.getJSON(url, vars, function (result) {
    if (result.count > 0) {
      Drupal.tingSearch.summary.ting = { count: result.count, page: result.page };
      $("#ting-search-spinner").hide("normal");

      // Add the template for ting result and facet browser.
      $("#ting-search-placeholder").replaceWith(Drupal.settings.tingSearch.result_template);

      // Pass the data on to the result and facet browser handlers.
      Drupal.tingResult("#ting-search-result", "#ting-facet-browser", result);
      Drupal.tingFacetBrowser("#ting-facet-browser", "#ting-search-result", result);
    }
    else {
      Drupal.tingSearch.summary.ting = { count: 0, page: 0 };
    }
    Drupal.tingSearch.updateTabs("ting");
  });
};

// Get search data from Drupal's content search
Drupal.tingSearch.getContentData = function(url, keys, show) {
  // Set up a params object to send along to getJSON.
  var params = {};
  // If we get new search keys via the keys parameter, they'll get
  // attached to the object here
  if (keys) {
    params.query = keys;
  }

  $.getJSON(url, params, function (data) {
    // Store some of the data returned on the tingSearch object in case
    // we should need it later.
    Drupal.tingSearch.summary.content = { count: data.count, page: data.page };
    Drupal.tingSearch.contentData = data;
    Drupal.tingSearch.updateTabs("content");

    if (data.count) {
      $("#content-search-result").html(Drupal.tingSearch.contentData.result_html);
      if (data.feed_icon) {
        if ($("#content-search-result .feed_icon").size() > 0) {
          $("#content-search-result .feed_icon").replaceWith(data.feed_icon);
        }
        else {
          $("#content-search-result").append(data.feed_icon);
        }
      }

      // Redo the click event bindings for the contentPager, since we'll
      // have a new pager from the result HTML.
      Drupal.tingSearch.contentPager();
      Drupal.tingSearch.updateSummary($('#content-search-summary'), data);

      // If the show parameter is specified, show our results.
      if (show) {

        $("#content-result").show("fast", function() {
          //jQuery.show adds style="display:block" after transition.
          //This conflicts with jQuery.tabs as it overrides styles from
          //classes and makes content results appear in other tabs even
          //if that tab is not selected.
          //Strip the entire style attribute to fix.
          //Note: This may cause problems if other code adds inline styles
          //without causing problems
          $(this).removeAttr('style');
        });
      }
    }
  });
};

// Redirect clicks on the pager to reload the content search.
Drupal.tingSearch.contentPager = function() {
  $("#content-result .pager a").click(function (eventObject) {
    $("#content-result").hide("fast");
    $("#ting-search-spinner").show("normal");
    Drupal.tingSearch.getContentData(this.href, false, true);
    return false;
  });
};

Drupal.tingSearch.tabLoading = function (sender) {
  if (Drupal.tingSearch.summary.hasOwnProperty(sender)) {
    var result, tab;
    tab = $('#ting-search-tabs li.' + sender);
    tab.addClass('spinning').find('span.count').remove();

    result = $("#" + sender + "-result");
    result.addClass('loading');
  }
};

// Helper function to update the state of our tabs.
Drupal.tingSearch.updateTabs = function (sender) {
  if (Drupal.tingSearch.summary.hasOwnProperty(sender)) {
    var tab, count, result;
    tab = $('#ting-search-tabs li.' + sender);
    count = Drupal.tingSearch.summary[sender].count;
    result = $("#" + sender + "-result");

    if (count == 0) {
      // For no results, replace the contents of the results container
      // with the no results message.
      result.html('<h4>' + Drupal.settings.tingResult.noResultsHeader + '</h4><p>' + Drupal.settings.tingResult.noResultsText + '</p>');
    }
    else if (sender == 'content') {
      // If we have a non-zero result count for content, enable its tab.
      $("#ting-search-tabs").tabs("enable", 1);
    }
    tab.removeClass('spinning');
    result.removeClass('loading');

    if (tab.find('span.count').length) {
      tab.find('span.count em').text(count);
    }
    else {
      tab.find('a').append(' <span class="count">(<em>' + count + '</em>)</span>');
    }
  }

  if (Drupal.tingSearch.summary.hasOwnProperty("ting") && Drupal.tingSearch.summary.hasOwnProperty("content")) {
    // When both searches has returned, make sure that we're in a
    // reasonably consistent state.
    $("#ting-search-spinner").hide("normal");

    // If there were no results from Ting and website results available,
    // switch to the website tab and diable the Ting tab.
    if (Drupal.tingSearch.summary.ting.count == 0 && Drupal.tingSearch.summary.content.count > 0) {
      $("#ting-search-tabs")
        .tabs("select", 1)
        .tabs("disable", 0);
    }
  }
};

Drupal.tingSearch.updateSummary = function (element, result) {
  element.find('.count').text(result.count);
  element.find('.firstResult').text((result.page - 1) * result.resultsPerPage + 1);
  element.find('.lastResult').text(Math.min(result.count, result.page * result.resultsPerPage));
};

Drupal.tingSearch.selectTab = function (event, ui) {
  window.location.href = $(ui.tab).attr('href');

  if (window.location.href.lastIndexOf('#ting-result') > -1) {
    //facet browser elements do not have dimensions before their tab is shown
    //so wait bit before updating them
    window.setTimeout(function() {
      Drupal.resetFacetBrowser('#ting-facet-browser');
      Drupal.bindResizeEvent('#ting-facet-browser');
      Drupal.bindSelectEvent("#ting-facet-browser", "#ting-search-result");
      Drupal.resizeFacets('#ting-facet-browser');
    }, 250);
  }
};

Drupal.getAnchorVars = function() {
  anchorValues = {};

  anchor = jQuery.url.attr('anchor');
  anchor = (anchor == null) ? '' : anchor;

  anchor = anchor.split('&');
  for (a in anchor) {
    keyValue = anchor[a].split('=');
    if (keyValue.length > 1) {
      anchorValues[keyValue[0]] = keyValue[1];
    }
  }

  return anchorValues;
};

Drupal.setAnchorVars = function(vars) {
  anchorArray = new Array();
  for (v in vars) {
    anchorArray.push(v + '=' + vars[v]);
  }
  anchorString = anchorArray.join('&');
  window.location.hash = '#' + anchorString;
};

