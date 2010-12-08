<?php
// $Id$

/**
 * @file
 * Template to render a periodicals issues from Ting.
 */
?>
<?php if (!empty($issues)): ?>
  <ul id="periodical-issue-list">
    <?php foreach ($issues as $year => $issue_list): ?>
    <li class="year year-<?php print check_plain($year); ?> clear-block">
      <?php print $issue_list; ?>
    </li>
    <?php endforeach; ?>
  </ul>
<?php else: ?>
  <p><?php print t('No issues found.'); ?></p>
<?php endif; ?>
