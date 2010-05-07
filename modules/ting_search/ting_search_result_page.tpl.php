<?php
// $Id$

/**
 * @file ting_result_page.tpl.php
 * Template for the search result page itself.
 */
?>
<div id="ting-search-results">

  <ul id="ting-search-tabs" class="ui-tabs-nav">
    <li class="ting spinning"><a href="#ting-result"><?php echo t('Materials') ?></a></li>
    <li class="content spinning"><a href="#content-result"><?php echo t('Website') ?></a></li>
  </ul>

  <div id="ting-result">
    <div id="ting-search-placeholder"></div>
  </div>

  <div id="content-result" class="ui-tabs-hide"></div>
</div>
<div id="ting-search-spinner">
  <h4><?php print t('Searching'); ?>…</h4>
  <div class="spinner"></div>
</div>

