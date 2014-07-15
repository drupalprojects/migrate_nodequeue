<?php

/**
 * @file
 * Wizard implementation for nodequeue migrations.
 */

/*
 * Implementation of hook_migrate_api().
 */
function migrate_nodequeue_migrate_api() {
  $api = array(
    'api' => 2,
    'wizard classes' => array('MigrateNodequeueWizard'),
  );
  return $api;
}

// TODO ability to re-use node migration for mappings.
// TODO ability to re-use role migration for mappings.
// TODO should work only with D6 sources.
// TODO does it work when nodequeue not enabled on d6 ?
// TODO import variables ?
class MigrateNodequeueWizard extends MigrateD2DWizard {

  /**
   * Number of queues to migrate.
   */
  protected $queuesCount;

  /**
   * Lay out the steps of the wizard.
   */
  public function __construct() {
    parent::__construct();
    $this->addStep(t('Credentials'), 'sourceDataForm');
    $this->addStep(t('Overview'), 'nodequeueOverview');
  }

  /**
   * Identify ourselves.
   *
   * @return string
   */
  public function getSourceName() {
    return t('Drupal Nodequeues');
  }

  /**
   * Present information and options around migrations.
   */
  protected function nodequeueOverview(&$form_state) {
    $form['#tree'] = TRUE;
    if (!isset($this->queuesCount)) {
      $this->queuesCount = $this->connection()
        ->select('nodequeue_queue', 'q')
        ->countQuery()
        ->execute()
        ->fetchField();
    }
    $form['queues'] = array(
      '#prefix' => '<div>',
      '#markup' => t('Number of queues to be migrated from your Drupal !version site: !count', array('!version' => $this->sourceVersion, '!count' => $this->queuesCount)),
      '#postfix' => '</div>',
    );
    return $form;
  }

  /**
   * Prepare for registration.
   *
   * @param array $form_state
   */
  protected function nodequeueOverviewValidate(&$form_state) {
    // TODO if checkbox not to migrate data, record that.
  }

  /**
   * Show the user what they've chosen to migrate, and give them one last chance
   * to say yay or nay.
   */
  protected function reviewForm(&$form_state) {
    $this->groupArguments = array(
      'encrypted_arguments' => array('source_database'),
      'source_version' => $this->sourceVersion,
      'source_connection' => $this->groupName . '_legacy',
      'source_database' => $this->database,
      'source_system' => t('Drupal @version Nodequeues', array('@version' => $this->sourceVersion)),
    );

    $form['description'] = array(
      '#prefix' => '<div>',
      '#markup' => t('<p>Please review your migration settings. When you submit this form, a migration job containing a migration task for each type of item to be imported will be created and you will be left at the dashboard.</p>'),
      '#suffix' => '</div>',
    );

    // TODO do not do that if module disabled in d6
    // TODO make sure $arguments['descrition'] is set in the mgiratyion class
    $this->addMigration('Queues', "NodequeueQueueDrupal{$this->sourceVersion}Migration", array());

    $form['import_list'] = array(
      '#prefix' => '<ul>',
      '#markup' => $imports, // TODO
      '#suffix' => '</ul><br />',
    );

    $form['non_import_list'] = array(
      '#prefix' => '<ul>',
      '#markup' => $non_imports, // TODO
      '#suffix' => '</ul>',
    );

    return $form;
  }

}

