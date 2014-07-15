<?php

/**
 * @file
 * Wizard implementation for nodequeue migrations.
 */

// TODO ability to re-use node migration for mappings.
// TODO ability to re-use role migration for mappings.
// TODO should work only with D6 sources.
// TODO does it work when nodequeue not enabled on d6 ?
// TODO import variables ?
class MigrateNodequeueWizard extends MigrateD2DWizard {

  /**
   * Lay out the steps of the wizard.
   */
  public function __construct() {
    $this->addStep(t('Credentials'), 'sourceDataForm');
    $this->addStep(t('Configuration'), 'nodequeueConfig');
  }

  /**
   * Identify ourselves.
   *
   * @return string
   */
  public function getSourceName() {
    return t('Nodequeue');
  }

  /**
   * Present information and options around migrations.
   */
  protected function nodequeueConfig(&$form_state) {
    //$form['#tree'] = TRUE; // TODO remove
    $queues = $this->connection()
      ->select('nodequeue_queue', 'q')
      ->fields('q', array('qid', 'title'))
      ->execute()
      ->fetchAllKeyed(0);
    foreach ($queues as $qid => $title) {
      $nodes = $this->connection() // TODO use a subquery ?
        ->select('nodequeue_nodes', 'n')
        ->condition('qid', $qid)
        ->countQuery()
        ->execute()
        ->fetchField();
      $subqueues = $this->connection() // TODO use a subquery ?
        ->select('nodequeue_subqueue', 's')
        ->condition('qid', $qid)
        ->countQuery()
        ->execute()
        ->fetchField();
      $queues[$qid] = array(
        'title' => $title,
        'subqueues' => $subqueues,
        'nodes' => $nodes,
      );
    }
    $form['queues'] = array(
      '#type' => 'tableselect',
      '#header' => array('title' => t('Queue Title'), 'subqueues' => t('Number of Subqueues'), 'nodes' => t('Number of Nodes')),
      '#options' => $queues,
      '#empty' => t('No queue found on your Drupal!version site.', array('!version' => $this->sourceVersion)),
    );
    return $form;
  }

  /**
   * Prepare for registration.
   *
   * @param array $form_state
   */
  protected function nodequeueConfigValidate(&$form_state) {
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

