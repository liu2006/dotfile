;; -*- lexical-binding: t; -*-
(setq inhibit-startup-screen t)
(tool-bar-mode 0)
(menu-bar-mode 0)
(scroll-bar-mode 0)
(ido-mode 1)
(setq ido-enable-flex-matching t)
(setq ido-everywhere t)
(global-display-line-numbers-mode 1)
(setq-default display-line-numbers-type 'relative)
(set-face-attribute 'default nil
                    :family "CodeNewRomanNerdFont"
                    :height 190
                    :width 'regular)

(add-to-list 'load-path "~/.emacs.local/themes")

(add-to-list 'custom-theme-load-path
             "~/.emacs.local/themes")
(load-theme 'sanityinc-tomorrow-night t)

(add-to-list 'load-path "~/.emacs.local/smex")
(require 'smex)
(smex-initialize)
(global-set-key (kbd "M-x") 'smex)
(global-set-key (kbd "M-X") 'smex-major-mode-commands)

(use-package magit
  :ensure t
  )

(use-package multiple-cursors
  :defer t)
(global-set-key (kbd "C-S-c C-S-c") 'mc/edit-lines)
(global-set-key (kbd "C->") 'mc/mark-next-like-this)
(global-set-key (kbd "C-<") 'mc/mark-previous-like-this)
(global-set-key (kbd "C-c C-<") 'mc/mark-all-like-this)

(setq treesit-language-source-alist
      '((bash "https://github.com/tree-sitter/tree-sitter-bash")
	(c "https://github.com/tree-sitter/tree-sitter-c")
	(c++ "https://github.com/tree-sitter/tree-sitter-cpp")
	(cmake "https://github.com/uyha/tree-sitter-cmake")
	(css "https://github.com/tree-sitter/tree-sitter-css")
	(html "https://github.com/tree-sitter/tree-sitter-html")
	(json "https://github.com/tree-sitter/tree-sitter-json")
	(toml "https://github.com/tree-sitter/tree-sitter-toml")
	(yaml "https://github.com/ikatyang/tree-sitter-yaml")))

(setq major-mode-remap-alist
      '((yaml-mode . yaml-ts-mode)
	(html-mode . html-ts-mode)
	(sh-mode . bash-ts-mode)
	(c-mode . c-ts-mode)
	(c++-mode . c++-ts-mode)
	(json-mode . json-ts-mode)
	(cmake-mode . cmake-ts-mode)
	(css-mode . css-ts-mode)))

;; (customize-set-variable 'treesit-font-lock-level 4)

(with-eval-after-load 'eglot
  (add-to-list 'eglot-server-programs
               '((c-ts-mode c++-ts-mode) . ("clangd")))
  (add-to-list 'eglot-server-programs
               '(bash-ts-mode . ("bash-language-server")))
  (add-to-list 'eglot-server-programs
               '(cmake-ts-mode . ("cmake-language-server")))
  (add-to-list 'eglot-server-programs
               '(yaml-ts-mode . ("yaml-language-server")))
  (add-to-list 'eglot-server-programs
               '(json-ts-mode . ("vscode-langservers-extracted")))
  (add-to-list 'eglot-server-programs
               '(css-ts-mode . ("vscode-langservers-extracted")))
  (add-to-list 'eglot-server-programs
               '(html-ts-mode . ("vscode-langservers-extracted")))
  
  )

(add-hook 'c-ts-mode-hook 'eglot-ensure)
(add-hook 'c++-ts-mode-hook 'eglot-ensure)
(add-hook 'cmake-ts-mode-hook 'eglot-ensure)
(add-hook 'css-ts-mode-hook 'eglot-ensure)
(add-hook 'html-ts-mode-hook 'eglot-ensure)
(add-hook 'json-ts-mode-hook 'eglot-ensure)
(add-hook 'taml-ts-mode-hook 'eglot-ensure)

(add-hook 'eglot-managed-mode-hook
          (lambda ()
            (eglot-inlay-hints-mode 0)))

(use-package company
  :ensure t
  )
(add-hook 'after-init-hook 'global-company-mode)
(setq company-idle-delay nil)
(global-set-key (kbd "M-TAB") 'company-complete)

(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(package-selected-packages '(magit multiple-cursors)))
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 )
